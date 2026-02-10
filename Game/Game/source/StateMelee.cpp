#include "StateMelee.h"
#include "Enemy.h"

namespace 
{
	// 攻撃コリジョン設定用定数
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 100.0f;	// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 60.0f;	// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;		// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;		// 攻撃コリジョン半径
	
	// 突進攻撃コリジョン設定用定数
	constexpr auto RUSH_COLLISION_OFFSET_Z = 50.0f;		// 突進攻撃コリジョン前方オフセット
	constexpr auto RUSH_COLLISION_OFFSET_Y = 80.0f;		// 突進攻撃コリジョンY位置オフセット
	constexpr auto RUSH_COLLISION_HEIGHT = 100.0f;		// 突進攻撃コリジョン高さ
	constexpr auto RUSH_COLLISION_RADIUS = 50.0f;		// 突進攻撃コリジョン半径

	// 距離判定用定数
	constexpr auto NEARBY_HOME = 10.0f;					// 初期位置到達判定距離

	constexpr auto WANDER_HOME_RETURN_RATIO = 0.8f;		// 徘徊時の帰還判定比率

	constexpr auto CONFRONT_ENTER_DISTANCE = 300.0f;	// 対峙状態への遷移距離
	constexpr auto CONFRONT_KEEP_DISTANCE = 250.0f;		// 対峙時の維持距離
	constexpr auto CONFRONT_DISTANCE_TOLERANCE = 30.0f;	// 対峙時の距離許容範囲

	constexpr auto ATTACK_START_DISTANCE = 200.0f;		// 攻撃開始距離
	constexpr auto ATTACK_EXECUTE_DISTANCE = 150.0f;	// 攻撃実行可能距離

	constexpr auto RUSH_TARGET_REACHED_DISTANCE = 5.0f;	// 突進攻撃ターゲット到達判定距離
	
	// 時間制御用定数
	constexpr auto ATTACK_CHARGE_TIME = 120.0f;			// 攻撃溜め時間
	constexpr auto ATTACK_EXECUTE_TIME = 90.0f;			// 攻撃実行時間
	constexpr auto ATTACK_RECOVERY_TIME = 180.0f;		// 攻撃後隙時間

	constexpr auto RUSH_CHARGE_TIME = 30.0f;			// 突進攻撃溜め時間
	constexpr auto RUSH_EXECUTE_MAX_TIME = 120.0f;		// 突進攻撃最大実行時間
	constexpr auto RUSH_WAIT_AFTER_REACH = 30.0f;		// 突進攻撃到達後の待機時間
	constexpr auto RUSH_RECOVERY_TIME = 90.0f;			// 突進攻撃後隙時間

	constexpr auto CONFRONT_MIN_DURATION = 90.0f;		// 対峙最小時間
	constexpr auto CONFRONT_RANDOM_DURATION = 90.0f;	// 対峙ランダム追加時間

	constexpr auto ATTACK_DURATION = 60.0f;				// 攻撃持続時間
	constexpr auto ATTACK_RECOVERY = 60.0f;				// 攻撃後隙時間
	constexpr auto ATTACK_DAMAGE = 1.0f;				// 攻撃ダメージ量
	constexpr auto RUSH_DAMAGE = 1.0f;					// 突進攻撃ダメージ量

	constexpr auto BLEND_FRAME = 1.0f;					// アニメーションブレンドフレーム数
	
	// 確率制御用定数
	constexpr auto CONFRONT_PROBABILITY = 40;			// 対峙状態への遷移確率(%)
	constexpr auto RUSH_PROBABILITY = 40;				// 突進攻撃選択確率(%)

	constexpr auto WANDER_ANGLE_RANDOM_RANGE = 90;		// 徘徊時の角度ランダム範囲(度)
	constexpr auto WANDER_ANGLE_RANDOM_OFFSET = 45;		// 徘徊時の角度ランダムオフセット(度)
	constexpr auto WANDER_FULL_ANGLE = 359;				// 徘徊時の全角度範囲(度)
	
	// 速度制御用定数
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度

	constexpr auto CONFRONT_CIRCLE_SPEED = 1.5f;		// 対峙の回り込み速度
	constexpr auto CONFRONT_APPROACH_SPEED = 0.5f;		// 対峙時の接近速度
	constexpr auto CONFRONT_RETREAT_SPEED = 1.0f;		// 対峙時の後退速度

	constexpr auto ATTACK_APPROACH_SPEED = 2.0f;		// 攻撃開始時の接近速度

	constexpr auto RUSH_INITIAL_SPEED = 10.0f;			// 突進攻撃初速
	constexpr auto RUSH_DECELERATION_RATE = 0.95f;		// 突進攻撃減速率
	constexpr auto RUSH_MIN_SPEED_THRESHOLD = 0.1f;		// 突進攻撃最小速度閾値(これ以下で到達とみなす)
	
	// アニメーション制御用定数
	constexpr auto ANIM_LOOP_COUNT = 0;					// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;					// アニメーション再生回数
	constexpr auto ANIM_SPEED_NORMAL = 1.0f;			// アニメーション再生速度(通常)
	
	// コリジョン初期値用定数
	constexpr auto COLLISION_DELAY_ZERO = 0.0f;			// コリジョン発生遅延なし

	// 攻撃コリジョン設定生成
	EnemyAttackSettings MakeMeleeAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y + ATTACK_COLLISION_HEIGHT, ATTACK_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y, ATTACK_COLLISION_OFFSET_Z);
		settings.fRadius = ATTACK_COLLISION_RADIUS;
		settings.fDelay = COLLISION_DELAY_ZERO;
		settings.fDuration = ATTACK_DURATION;
		settings.fRecovery = ATTACK_RECOVERY;
		settings.fDamage = ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	// 突進攻撃コリジョン設定生成
	EnemyAttackSettings MakeRushSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, RUSH_COLLISION_OFFSET_Y + RUSH_COLLISION_HEIGHT, RUSH_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, RUSH_COLLISION_OFFSET_Y, RUSH_COLLISION_OFFSET_Z);
		settings.fRadius = RUSH_COLLISION_RADIUS;
		settings.fDelay = COLLISION_DELAY_ZERO;
		settings.fDuration = ATTACK_DURATION;
		settings.fRecovery = ATTACK_RECOVERY;
		settings.fDamage = RUSH_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	// 定数として保存
	const EnemyAttackSettings MELEE_ATTACK_SETTINGS = MakeMeleeAttackSettings();
	const EnemyAttackSettings RUSH_SETTINGS = MakeRushSettings();
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
		float dist = VSize(vToTarget);
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
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
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
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}

		// 移動方向計算
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
		float dist = VSize(vToHome);
		float fLimitRange = owner->GetEnemyParam().fMoveRadius;

		float fTargetAngle = 0.0f;

		// 初期位置からの距離に応じて方向決定
		if (dist > fLimitRange * WANDER_HOME_RETURN_RATIO)
		{
			// 初期位置方向へ向かう
			float fToHomeAngle = atan2f(vToHome.z, vToHome.x);
			float fRandOffset = static_cast<float>(GetRand(WANDER_ANGLE_RANDOM_RANGE) - WANDER_ANGLE_RANDOM_OFFSET) * DEGREE_TO_RADIAN;
			fTargetAngle = fToHomeAngle + fRandOffset;
		}
		else 
		{
			// ランダム方向へ向かう
			fTargetAngle = static_cast<float>(GetRand(WANDER_FULL_ANGLE)) * DEGREE_TO_RADIAN;
		}

		// 方向ベクトル設定
		VECTOR vDir = VGet(cosf(fTargetAngle), 0.0f, sinf(fTargetAngle));
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
		float distSq = VSquareSize(vFromHome);
		float fLimitRange = owner->GetEnemyParam().fMoveRadius;
		
		if (distSq > fLimitRange * fLimitRange) 
		{
			return std::make_shared<ReturnHome>();
		}

		// 移動処理
		VECTOR vDir = owner->GetDir();
		float fSpeed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, fSpeed);
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
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> Notice::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// ターゲット方向へ回転処理
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
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, ANIM_LOOP_COUNT);
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
		float dist = VSize(vToTarget);
		const auto& param = owner->GetEnemyParam();
		
		// 追跡限界距離チェック
		if(dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// 対峙ステートになるかチェック
		if (dist <= CONFRONT_ENTER_DISTANCE)
		{
			if (!_bHasChecked)
			{
				// 確率で対峙か接近継続かを判定
				if (GetRand(100) < CONFRONT_PROBABILITY)
				{
					return std::make_shared<Confront>();
				}
				_bHasChecked = true;
			}
		}

		// 攻撃開始距離チェック
		if (dist <= ATTACK_START_DISTANCE)
		{
			return std::make_shared<AttackStart>();
		}

		// ターゲット方向へ回転処理
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理
		float fSpeed = param.fMoveSpeed;
		VECTOR vMove = VScale(vDir, fSpeed);
		owner->SetMove(vMove);

		return nullptr;
	}





	// 攻撃ステート開始
	void AttackStart::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 攻撃タイプ決定処理
		if (GetRand(100) < RUSH_PROBABILITY)
		{
			_eAttackType = AttackType::RUSH;
		}
		else
		{
			_eAttackType = AttackType::NORMAL;
		}

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> AttackStart::Update(Enemy* owner)
	{
		// ターゲット存在チェック	
		auto target = owner->GetTarget();
		if(!target)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// タイマー更新
		_fTimer++;

		// 距離計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);
		const auto& param = owner->GetEnemyParam();

		// 追跡限界距離チェック
		if (dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// 攻撃タイプ別の遷移処理
		if(_eAttackType == AttackType::RUSH)
		{
			// 突進攻撃へ
			return std::make_shared<RushCharge>();
		}
		else
		{
			// 通常攻撃:攻撃実行距離チェック
			if (dist <= ATTACK_EXECUTE_DISTANCE)
			{
				return std::make_shared<AttackCharge>();
			}
		}

		// ターゲット方向へ回転処理
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理
		VECTOR vMove = VScale(vDir, ATTACK_APPROACH_SPEED);
		owner->SetMove(vMove);

		return nullptr;
	}

	// 攻撃溜め
	void AttackCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> AttackCharge::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止処理
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// ターゲット方向へ追従処理
		auto target = owner->GetTarget();
		if (target)
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);
		}

		// 溜め時間終了チェック
		if (_fTimer >= ATTACK_CHARGE_TIME)
		{
			return std::make_shared<AttackExecute>();
		}

		return nullptr;
	}

	// 攻撃実行
	void AttackExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bHasCollision = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_NORMAL);
		}

		// 攻撃コリジョン生成
		owner->StartAttack(MELEE_ATTACK_SETTINGS);
		_bHasCollision = true;
	}

	std::shared_ptr<EnemyState> AttackExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止処理
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 攻撃コリジョン生成
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(MELEE_ATTACK_SETTINGS);
		}

		// 攻撃実行時間終了チェック
		if (_fTimer >= ATTACK_EXECUTE_TIME)
		{
			return std::make_shared<AttackRecovery>();
		}

		return nullptr;
	}

	void AttackExecute::Exit(Enemy* owner)
	{
		// 攻撃コリジョン停止処理
		if (_bHasCollision)
		{
			owner->StopAttack();
			_bHasCollision = false;
		}
	}

	// 攻撃後隙
	void AttackRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> AttackRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止処理
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 後隙時間終了チェック
		if (_fTimer >= ATTACK_RECOVERY_TIME)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}

	// 突進攻撃溜め
	void RushCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ターゲット方向保存
		auto target = owner->GetTarget();
		if (target)
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			_vRushDir = VNorm(vToTarget);
		}
		else
		{
			_vRushDir = owner->GetDir();
		}

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> RushCharge::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止処理
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// ターゲット方向へ追従処理
		auto target = owner->GetTarget();
		if (target)
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);
			_vRushDir = vDir;
		}

		// 溜め時間終了チェック
		if (_fTimer >= RUSH_CHARGE_TIME)
		{
			return std::make_shared<RushExecute>(_vRushDir);
		}

		return nullptr;
	}

	// 突進攻撃実行
	void RushExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fReachTimer = 0.0f;
		_bHasCollision = false;
		_fCurrentSpeed = RUSH_INITIAL_SPEED;
		_bReachedTarget = false;

		// 水平方向のみの移動にする
		_vRushDir.y = 0.0f;
		_vRushDir = VNorm(_vRushDir);

		// 突進開始位置を保存
		_vStartPos = owner->GetPos();

		// ターゲット位置を保存
		auto target = owner->GetTarget();
		if (target)
		{
			_vTargetPos = target->GetPos();
			_vTargetPos.y = owner->GetPos().y;// Y座標は合わせる

			// 突進方向を再計算
			VECTOR vToTarget = VSub(_vTargetPos, owner->GetPos());
			vToTarget.y = 0.0f;
			float dist = VSize(vToTarget);

			if (dist > 0.01f)
			{
				_vRushDir = VNorm(vToTarget);
			}
		}
		else
		{
			// ターゲットがいない場合は現在の向き + 適当な距離
			_vTargetPos = VAdd(owner->GetPos(), VScale(_vRushDir, 300.0f));
		}

		// 方向設定
		owner->SetDir(_vRushDir);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_NORMAL);
		}

		// 突進攻撃コリジョン生成
		owner->StartAttack(RUSH_SETTINGS);
		_bHasCollision = true;
	}

	std::shared_ptr<EnemyState> RushExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 現在位置取得
		VECTOR vCurrentPos = owner->GetPos();

		// ターゲット位置までの距離計算
		VECTOR vToTarget = VSub(_vTargetPos, vCurrentPos);
		vToTarget.y = 0.0f;
		float distToTarget = VSize(vToTarget);

		// ターゲット位置に到達したかチェック
		if (distToTarget <= RUSH_TARGET_REACHED_DISTANCE || _bReachedTarget || _fCurrentSpeed < RUSH_MIN_SPEED_THRESHOLD)
		{
			if (!_bReachedTarget)
			{
				_bReachedTarget = true;
				_fReachTimer = 0.0f;
			}

			// 到達後タイマー更新
			_fReachTimer++;

			// 到達後は停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

			// 突進攻撃コリジョン更新
			if (_bHasCollision)
			{
				owner->UpdateAttackTransform(RUSH_SETTINGS);
			}

			// 到達後一定時間経過で後隙へ
			if (_fReachTimer >= RUSH_WAIT_AFTER_REACH)
			{
				return std::make_shared<RushRecovery>();
			}

			return nullptr;
		}

		// 速度減衰処理
		_fCurrentSpeed *= RUSH_DECELERATION_RATE;

		// 残り距離が速度より小さいなら距離分だけ移動
		float moveDistance = _fCurrentSpeed;
		if (distToTarget < moveDistance)
		{
			moveDistance = distToTarget;
		}

		// 突進移動処理
		VECTOR vMove = VScale(_vRushDir, moveDistance);
		owner->SetMove(vMove);

		// 突進攻撃コリジョン更新
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(RUSH_SETTINGS);
		}

		// 最大時間経過チェック
		if (_fTimer >= RUSH_EXECUTE_MAX_TIME)
		{
			return std::make_shared<RushRecovery>();
		}

		return nullptr;
	}

	void RushExecute::Exit(Enemy* owner)
	{
		// 突進攻撃コリジョン停止処理
		if (_bHasCollision)
		{
			owner->StopAttack();
			_bHasCollision = false;
		}
	}

	// 突進攻撃後隙
	void RushRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> RushRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止処理
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 硬直時間終了チェック
		if (_fTimer >= RUSH_RECOVERY_TIME)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}





	// 帰還
	void ReturnHome::Enter(Enemy* owner)
	{
		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, ANIM_LOOP_COUNT);
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
		float dist = VSize(vToHome);

		// 到達判定チェック
		if (dist <= NEARBY_HOME) 
		{
			return std::make_shared<Idle>();
		}

		// 初期位置方向へ回転処理
		VECTOR vDir = VNorm(vToHome);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理
		float fSpeed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, fSpeed);
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
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, ANIM_LOOP_COUNT);
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

		// 追跡限界距離チェック
		if (dist > param.fChaseLimitRange)
		{
			return std::make_shared<Approach>();
		}

		// 攻撃開始距離チェック
		if (dist <= ATTACK_START_DISTANCE)
		{
			return std::make_shared<AttackStart>();
		}

		// ターゲット方向の正規化ベクトル計算
		VECTOR vToTargetNorm = VNorm(vToTarget);

		// 上方向ベクトル
		VECTOR vUp = VGet(0.0f, 1.0f, 0.0f);

		// 横方向ベクトル計算
		VECTOR vRight = VCross(vToTargetNorm, vUp);

		// 距離差分計算
		float fDistanceDiff = dist - CONFRONT_KEEP_DISTANCE;

		// 距離調整ベクトル計算
		VECTOR vDistanceAdjust = VGet(0.0f, 0.0f, 0.0f);
		if (fDistanceDiff > CONFRONT_DISTANCE_TOLERANCE)
		{
			// 遠すぎる場合は接近
			vDistanceAdjust = VScale(vToTargetNorm, CONFRONT_APPROACH_SPEED);
		}
		else if (fDistanceDiff < -CONFRONT_DISTANCE_TOLERANCE)
		{
			// 近すぎる場合は後退
			vDistanceAdjust = VScale(vToTargetNorm, -CONFRONT_RETREAT_SPEED);
		}

		// 回り込みベクトル計算
		VECTOR vCircle = VScale(vRight, _direction * CONFRONT_CIRCLE_SPEED);

		// 合成移動ベクトル計算
		VECTOR vMove = VAdd(vDistanceAdjust, vCircle);

		// ターゲット方向設定
		owner->SetDir(vToTargetNorm);

		// 移動処理
		owner->SetMove(vMove);

		// 時間経過チェック
		_fTimer++;
		if (_fTimer >= _fDuration)
		{
			return std::make_shared<AttackStart>();
		}

		return nullptr;
	}
}
