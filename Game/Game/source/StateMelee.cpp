#include "StateMelee.h"
#include "Enemy.h"

namespace 
{
	// 攻撃コリジョン設定用定数
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 100.0f;	// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 60.0f;	// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;		// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;		// 攻撃コリジョン半径
	
	// 距離判定用定数
	constexpr auto NEARBY_HOME = 10.0f;					// 初期位置到達判定距離
	constexpr auto WANDER_HOME_RETURN_RATIO = 0.8f;		// 徘徊時の帰還判定比率
	constexpr auto CONFRONT_DISTANCE_RATIO = 2.0f;		// 対峙時の接近判定比率
	
	// 時間制御用定数
	constexpr auto ATTACK_CONFIRM_TIME = 60.0f;			// 攻撃判定が有効になるまでの時間
	constexpr auto BLEND_FRAME = 1.0f;					// アニメーションブレンドフレーム数
	constexpr auto CONFRONT_MIN_DURATION = 60.0f;		// 対峙最小時間
	constexpr auto CONFRONT_RANDOM_DURATION = 60.0f;	// 対峙ランダム追加時間
	
	// 確率制御用定数
	constexpr auto CONFRONT_PROBABILITY = 30;			// 対峙状態への遷移確率(%)
	constexpr auto WANDER_ANGLE_RANDOM_RANGE = 90;		// 徘徊時の角度ランダム範囲(度)
	constexpr auto WANDER_ANGLE_RANDOM_OFFSET = 45;		// 徘徊時の角度ランダムオフセット(度)
	constexpr auto WANDER_FULL_ANGLE = 359;				// 徘徊時の全角度範囲(度)
	
	// 回転速度用定数
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度

	constexpr auto CONFRONT_FORWARD_SPEED = 1.0f;// 対峙の前進速度
	constexpr auto COMFRONT_CIRCLE_SPEED = 1.5f;// 対峙の回り込み速度

	// 攻撃コリジョン設定生成
	EnemyAttackSettings MakeMeleeAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y + ATTACK_COLLISION_HEIGHT, ATTACK_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y, ATTACK_COLLISION_OFFSET_Z);
		settings.fRadius = ATTACK_COLLISION_RADIUS;
		settings.fDelay = 0.0f;
		settings.fDuration = 60.0f;
		settings.fRecovery = 60.0f;
		settings.fDamage = 10.0f;
		settings.ownerId = 0;
		return settings;
	}

	// 定数として保存
	const EnemyAttackSettings MELEE_ATTACK_SETTINGS = MakeMeleeAttackSettings();
}

namespace Melee
{
	// 視界判定
	bool IsTargetVisible(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// ベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		
		// 距離チェック
		auto dist = VSize(vToTarget);
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 障害物チェック(未実装)

		return true;
	}





	// 待機
	void Idle::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間にばらつきを持たせる
		_fTargetTimer = CalcOffsetTime(owner, owner->GetEnemyParam().fIdleTime);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) 
	{
		// 索敵結果チェック
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Notice>();
		}

		// タイマー更新
		_fTimer++;

		// 時間経過チェック
		if(_fTimer >= _fTargetTimer)
		{
			return std::make_shared<Wander>();
		}

		return nullptr;
	}

	void Idle::UpdateSearch(Enemy* owner)
	{
		// 視界判定結果を設定
		owner->SetTargetDetected(Melee::IsTargetVisible(owner));
	}





	// 徘徊
	void Wander::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}

		// 移動方向計算
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
		auto dist = VSize(vToHome);
		auto limitRange = owner->GetEnemyParam().fMoveRadius;

		float targetAngle = 0.0f;

		// 初期位置からの距離に応じて方向決定
		if (dist > limitRange * WANDER_HOME_RETURN_RATIO)
		{
			// 初期位置方向へ向かう
			auto toHomeAngle = atan2f(vToHome.z, vToHome.x);
			auto randOffset = static_cast<float>(GetRand(WANDER_ANGLE_RANDOM_RANGE) - WANDER_ANGLE_RANDOM_OFFSET) * DEGREE_TO_RADIAN;
			targetAngle = toHomeAngle + randOffset;
		}
		else 
		{
			// ランダム方向へ向かう
			targetAngle = static_cast<float>(GetRand(WANDER_FULL_ANGLE)) * DEGREE_TO_RADIAN;
		}

		// 方向ベクトル設定
		VECTOR vDir = VGet(cosf(targetAngle), 0.0f, sinf(targetAngle));
		owner->SetDir(vDir);
	}

	std::shared_ptr<EnemyState> Wander::Update(Enemy* owner)
	{
		// 索敵結果チェック
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Notice>();
		}

		// タイマー更新
		_fTimer++;

		// 移動範囲チェック
		VECTOR vFromHome = VSub(owner->GetPos(), owner->GetHomePos());
		auto distSq = VSquareSize(vFromHome);
		auto limitRange = owner->GetEnemyParam().fMoveRadius;
		
		if (distSq > limitRange * limitRange) 
		{
			return std::make_shared<ReturnHome>();
		}

		// 移動処理
		VECTOR vDir = owner->GetDir();
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);

		// 時間経過チェック
		if (_fTimer >= owner->GetEnemyParam().fMoveTime) 
		{
			return std::make_shared<Idle>();
		}

		return nullptr;
	}

	void Wander::UpdateSearch(Enemy* owner)
	{
		// 視界判定結果を設定
		owner->SetTargetDetected(Melee::IsTargetVisible(owner));
	}





	// 発見
	void Notice::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Notice::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// ターゲット方向へ回転
		auto target = owner->GetTarget();
		if (target) 
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			owner->SetDir(vDir);
		}

		// 時間経過チェック
		if (_fTimer >= owner->GetEnemyParam().fDetectTime)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}





	// 接近
	void Approach::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		_bHasChecked = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Approach::Update(Enemy* owner) 
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target) 
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// 距離計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		auto dist = VSize(vToTarget);
		const auto& param = owner->GetEnemyParam();
		
		// 追跡限界距離チェック
		if(dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// 対峙ステートになるかチェック
		if(dist <= param.fAttackRange * CONFRONT_DISTANCE_RATIO)
		{
			if (!_bHasChecked)// 一度のみ判定
			{
				// 確率で対峙か接近継続かを判定
				if (GetRand(100) < CONFRONT_PROBABILITY)
				{
					return std::make_shared<Confront>();
				}
				_bHasChecked = true;
			}
		}

		// 攻撃射程チェック
		if (dist <= param.fAttackRange)
		{
			return std::make_shared<Attack>();
		}

		// ターゲット方向へ回転
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理
		auto speed = param.fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);

		return nullptr;
	}





	// 攻撃
	void Attack::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bHasCollision = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Attack::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 溜め時間中の処理
		if (_fTimer < ATTACK_CONFIRM_TIME)
		{
			// ターゲット方向へ追従
			auto target = owner->GetTarget();
			if (target)
			{
				VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
				VECTOR vDir = VNorm(vToTarget);
				owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);
			}
		}
		// 攻撃確定処理
		else if (!_bHasCollision)
		{
			// アニメーション切り替え
			AnimManager* animManager = owner->GetAnimManager();
			if (animManager)
			{
				animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);
			}

			// 攻撃コリジョン生成
			owner->StartAttack(MELEE_ATTACK_SETTINGS);
			_bHasCollision = true;
		}

		// 攻撃コリジョン更新
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(MELEE_ATTACK_SETTINGS);
		}

		// 攻撃終了チェック
		if (_fTimer >= owner->GetEnemyParam().fAttackTime)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}

	void Attack::Exit(Enemy* owner) 
	{
		// 攻撃コリジョン停止
		if (_bHasCollision)
		{
			owner->StopAttack();
			_bHasCollision = false;
		}
	}

	bool Attack::CanChangeState() 
	{
		// 攻撃中はステート変更不可
		return false;
	}





	// 帰還
	void ReturnHome::Enter(Enemy* owner)
	{
		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> ReturnHome::Update(Enemy* owner)
	{
		// 索敵結果チェック
		if(owner->IsTargetDetected())
		{
			return std::make_shared<Notice>();
		}

		// 初期位置への距離計算
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
		auto dist = VSize(vToHome);

		// 到達判定
		if (dist <= NEARBY_HOME) 
		{
			return std::make_shared<Idle>();
		}

		// 初期位置方向へ回転
		VECTOR vDir = VNorm(vToHome);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);

		return nullptr;
	}

	void ReturnHome::UpdateSearch(Enemy* owner)
	{
		// 視界判定結果を設定
		owner->SetTargetDetected(Melee::IsTargetVisible(owner));
	}





	// 対峙
	void Confront::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fDuration = CONFRONT_MIN_DURATION + GetRand(static_cast<int>(CONFRONT_RANDOM_DURATION));
		
		// 移動方向決定
		_direction = (GetRand(1) == 0) ? 1 : -1;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Confront::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// ターゲットへのベクトル計算
		VECTOR vTargetPos = target->GetPos();
		VECTOR vOwnerPos = owner->GetPos();
		VECTOR vToTarget = VSub(vTargetPos, vOwnerPos);
		vToTarget.y = 0.0f;

		// 距離計算
		float dist = VSize(vToTarget);
		const auto& param = owner->GetEnemyParam();

		// 距離チェック
		if (dist > param.fChaseLimitRange)
		{
			return std::make_shared<Approach>();
		}

		// ターゲット方向の正規化ベクトル
		VECTOR vToTargetNorm = VNorm(vToTarget);

		// 上方向ベクトル
		VECTOR vUp = VGet(0.0f, 1.0f, 0.0f);

		// 横方向ベクトル(ターゲット方向に対して垂直)
		VECTOR vRight = VCross(vToTargetNorm, vUp);

		// 前進ベクトル(ゆっくり近づく)
		VECTOR vForward = VScale(vToTargetNorm, CONFRONT_FORWARD_SPEED);

		// 回り込みベクトル
		VECTOR vCircle = VScale(vRight, _direction * COMFRONT_CIRCLE_SPEED);

		// 合成移動ベクトル
		VECTOR vMove = VAdd(vForward, vCircle);

		// ターゲット方向を向く
		owner->SetDir(vToTargetNorm);

		// 移動を適用
		owner->SetMove(vMove);

		// 時間経過チェック
		_fTimer++;
		if (_fTimer >= _fDuration)
		{
			return std::make_shared<Attack>();
		}

		return nullptr;
	}
}
