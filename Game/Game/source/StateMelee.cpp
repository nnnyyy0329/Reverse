#include "StateMelee.h"
#include "Enemy.h"

namespace 
{
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 100.0f;// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 60.0f;// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;// 攻撃コリジョン半径
	constexpr auto NEARBY_HOME = 10.0f;// 初期位置とみなす距離

	constexpr auto ATTACK_CONFIRM_TIME = 60.0f;// 攻撃判定が有効になるまでの時間

	constexpr auto BLEND_FRAME = 1.0f;// アニメーションブレンドフレーム数

	// 攻撃コリジョン設定
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
		settings.ownerId = 0;// 仮
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
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// 距離チェック
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());// ターゲットへのベクトル
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }// 索敵距離外

		// 障害物チェック

		return true;// 視界内
	}





	// 待機
	void Idle::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;

		// 時間にばらつきを持たせる
		_fTargetTimer = CalcOffsetTime(owner, owner->GetEnemyParam().fIdleTime);

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) 
	{
		// 索敵結果を使用
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Detect>();// 発見状態へ
		}

		_fTimer++;
		if(_fTimer >= _fTargetTimer)
		{
			return std::make_shared<Move>();// 自動移動状態へ
		}

		return nullptr;
	}

	void Idle::UpdateSearch(Enemy* owner)
	{
		if (Melee::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 自動移動
	void Move::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 歩きアニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);// 無限ループ
		}


		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());// 初期位置へのベクトル
		auto dist = VSize(vToHome);// 初期位置までの距離
		auto limitRange = owner->GetEnemyParam().fMoveRadius;

		auto targetAngle = 0.0f;

		// 初期位置からの距離によって方向を変える
		if (dist > limitRange * 0.8f)
		{// 80%以上離れていたなら、初期位置を向く
			auto toHomeAngle = atan2f(vToHome.z, vToHome.x);// 初期位置への角度
			// ランダムな角度を足して少しばらつかせる
			auto randOffset = static_cast<float>(GetRand(90) - 45) * DEGREE_TO_RADIAN;
			targetAngle = toHomeAngle + randOffset;
		}
		else 
		{
			// ランダムな方向に向く
			targetAngle = static_cast<float>(GetRand(359)) * DEGREE_TO_RADIAN;
		}

		VECTOR vDir = VGet(cosf(targetAngle), 0.0f, sinf(targetAngle));// XZ平面の角度の方向にある点のベクトルを計算
		owner->SetDir(vDir);
	}

	std::shared_ptr<EnemyState> Move::Update(Enemy* owner)
	{
		// 索敵結果を使用
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Detect>();// 発見状態へ
		}

		_fTimer++;

		// 移動範囲のチェック
		VECTOR vFromHome = VSub(owner->GetPos(), owner->GetHomePos());// 初期位置からのベクトル
		auto distSq = VSquareSize(vFromHome);// 平方根を使わない距離の2乗で
		auto limitRange = owner->GetEnemyParam().fMoveRadius;
		if (distSq > limitRange * limitRange) 
		{// 範囲外なら
			return std::make_shared<ReturnHome>();// 帰還状態へ
		}

		// 移動処理
		VECTOR vDir = owner->GetDir();
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().fMoveTime) 
		{
			return std::make_shared<Idle>();// 待機状態へ
		}
		return nullptr;
	}

	void Move::UpdateSearch(Enemy* owner)
	{
		if (Melee::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 発見
	void Detect::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 発見時の硬直(待機アニメーション)
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Detect::Update(Enemy* owner) 
	{
		_fTimer++;

		// ターゲットの方向を向く
		auto target = owner->GetTarget();
		if (target) 
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			// 即座に向きを変更
			owner->SetDir(vDir);
		}

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().fDetectTime)
		{
			return std::make_shared<Chase>();// 追跡状態へ
		}
		return nullptr;
	}





	// 追跡
	void Chase::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 歩きアニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Chase::Update(Enemy* owner) 
	{
		auto target = owner->GetTarget();
		// ターゲットがいない場合は待機状態へ
		if (!target) 
		{
			// 索敵結果をクリア
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		
		// 追跡限界距離を超えたか
		if(dist > owner->GetEnemyParam().fChaseLimitRange) 
		{
			// 索敵結果をクリア
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 攻撃射程内か
		if (dist <= owner->GetEnemyParam().fAttackRange) 
		{
			return std::make_shared<Attack>();// 攻撃状態へ
		}

		// 向きを変更
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, 5.0f);

		// 移動処理
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		return nullptr;
	}





	// 攻撃
	void Attack::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;
		_bHasCollision = false;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 攻撃前の溜め(待機アニメーション)
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Attack::Update(Enemy* owner) 
	{
		_fTimer++;

		// 攻撃中は移動しない
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// 溜め時間:ターゲットの方向を追従
		if (_fTimer < ATTACK_CONFIRM_TIME)
		{
			auto target = owner->GetTarget();
			if (target)
			{
				VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
				VECTOR vDir = VNorm(vToTarget);
				owner->SmoothRotateTo(vDir, 5.0f);
			
			}
		}
		// 攻撃確定:コリジョンを生成
		else if (!_bHasCollision)
		{
			// ここでアニメーション設定
			// AnimManagerを取得してアニメーション切り替え
			AnimManager* animManager = owner->GetAnimManager();
			if (animManager)
			{
				// 攻撃アニメーションに切り替え
				animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);// ループ無し
			}

			owner->StartAttack(MELEE_ATTACK_SETTINGS);
			_bHasCollision = true;
		}

		// 攻撃持続中:コリジョン位置更新
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(MELEE_ATTACK_SETTINGS);
		}

		// 攻撃終了
		if (_fTimer >= owner->GetEnemyParam().fAttackTime)
		{
			return std::make_shared<Chase>();// 追跡状態に戻ることで再度距離判定を行う
		}

		return nullptr;
	}

	void Attack::Exit(Enemy* owner) 
	{
		if (_bHasCollision)
		{
			owner->StopAttack();// 攻撃コリジョン停止
			_bHasCollision = false;
		}
	}

	bool Attack::CanChangeState() 
	{
		// 攻撃中は通常のステート変更は許可しない
		return false;
	}





	// 帰還
	void ReturnHome::Enter(Enemy* owner)
	{

		// ここでアニメーション設定
	// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 歩きアニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> ReturnHome::Update(Enemy* owner)
	{
		// 索敵結果を使用
		if(owner->IsTargetDetected())
		{
			return std::make_shared<Detect>();// 発見状態へ
		}

		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());// 初期位置へのベクトル
		auto dist = VSize(vToHome);// 初期位置までの距離

		// 初期位置に到達したか
		if (dist <= NEARBY_HOME) 
		{
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 向きを変更
		VECTOR vDir = VNorm(vToHome);
		owner->SmoothRotateTo(vDir, 5.0f);

		// 初期位置に向かって移動
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		return nullptr;
	}

	void ReturnHome::UpdateSearch(Enemy* owner)
	{
		if (Melee::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 回り込み
	void Strafe::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_fDuration = 60.0f + GetRand(60);// 1～2秒間
		_direction = (GetRand(1) == 0) ? 1 : -1;// ランダムに方向を決める

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Strafe::Update(Enemy* owner)
	{
	}
}
