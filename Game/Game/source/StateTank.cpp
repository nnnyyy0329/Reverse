#include "StateTank.h"
#include "Enemy.h"
#include "StateCommon.h"
#include "EnemyAttackSettings.h"

namespace 
{
	// 一段目攻撃
	constexpr auto FIRST_PREPARE_TIME = 40.0f;// 予備動作時間
	constexpr auto FIRST_ATTACK_DELAY = 5.0f;// 攻撃発生フレーム
	constexpr auto FIRST_ATTACK_DURATION = 20.0f;// 攻撃持続フレーム
	constexpr auto FIRST_ATTACK_RECOVERY = 10.0f;// 後隙フレーム
	constexpr auto FIRST_RECOVERY_TIME = 25.0f;// 攻撃後の硬直時間
	constexpr auto FIRST_ATTACK_DAMAGE = 1.0f;// ダメージ
	constexpr auto FIRST_APPROACH_SPEED = 4.0f;// 接近速度

	// 二段目攻撃
	constexpr auto SECOND_PREPARE_TIME = 35.0f;// 予備動作時間	
	constexpr auto SECOND_ATTACK_DELAY = 8.0f;// 攻撃発生フレーム
	constexpr auto SECOND_ATTACK_DURATION = 25.0f;// 攻撃持続フレーム
	constexpr auto SECOND_ATTACK_RECOVERY = 15.0f;// 後隙フレーム
	constexpr auto SECOND_RECOVERY_TIME = 45.0f;// 攻撃後の硬直時間	
	constexpr auto SECOND_ATTACK_DAMAGE = 1.0f;// ダメージ
	constexpr auto SECOND_APPROACH_SPEED = 5.0f;// 接近速度

	constexpr auto BLEND_FRAME = 1.0f;// アニメーションブレンドフレーム数
	constexpr auto NEARBY_HOME = 10.0f;// 初期位置とみなす距離

	// 攻撃コリジョン設定
	EnemyAttackSettings MakeFirstAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(50.0f, 120.0f, 100.0f);// 右手前方
		settings.vBottomOffset = VGet(50.0f, 30.0f, 100.0f);
		settings.fRadius = 70.0f;
		settings.fDelay = FIRST_ATTACK_DELAY;
		settings.fDuration = FIRST_ATTACK_DURATION;
		settings.fRecovery = FIRST_ATTACK_RECOVERY;
		settings.fDamage = FIRST_ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	EnemyAttackSettings MakeSecondAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(-50.0f, 130.0f, 110.0f);// 左手前方
		settings.vBottomOffset = VGet(-50.0f, 40.0f, 110.0f);
		settings.fRadius = 80.0f;
		settings.fDelay = SECOND_ATTACK_DELAY;
		settings.fDuration = SECOND_ATTACK_DURATION;
		settings.fRecovery = SECOND_ATTACK_RECOVERY;
		settings.fDamage = SECOND_ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}
	// 定数として保存
	const EnemyAttackSettings FIRST_ATTACK_SETTINGS = MakeFirstAttackSettings();
	const EnemyAttackSettings SECOND_ATTACK_SETTINGS = MakeSecondAttackSettings();
}

namespace Tank
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
	void Tank::Idle::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Tank::Idle::Update(Enemy* owner)
	{
		// 索敵結果を使用
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Tank::Detect>();// 発見状態へ
		}

		_fTimer++;

		if (_fTimer >= owner->GetEnemyParam().fIdleTime)
		{
			return std::make_shared<Tank::Move>();// 自動移動状態へ
		}

		return nullptr;
	}

	void Tank::Idle::UpdateSearch(Enemy* owner)
	{
		if (Tank::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 自動移動
	void Tank::Move::Enter(Enemy* owner)
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

		// 移動方向をランダムに決定
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());// 初期位置へのベクトル
		auto dist = VSize(vToHome);// 初期位置までの距離
		auto limitRange = owner->GetEnemyParam().fMoveRadius;

		auto targetAngle = 0.0f;

		if (dist > limitRange * 0.8f)
		{
			// 80%以上離れていたなら、初期位置を向く
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

		VECTOR vDir = VGet(cosf(targetAngle), 0.0f, sinf(targetAngle));	
		owner->SetDir(vDir);
	}

	std::shared_ptr<EnemyState> Tank::Move::Update(Enemy* owner)
	{
		// 索敵結果を使用
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Tank::Detect>();// 発見状態へ
		}

		_fTimer++;

		// 移動範囲チェック
		VECTOR vFromHome = VSub(owner->GetPos(), owner->GetHomePos());// 初期位置からのベクトル
		auto distSq = VSquareSize(vFromHome);// 平方根を使わない距離の2乗で	
		auto limitRange = owner->GetEnemyParam().fMoveRadius;
		if (distSq > limitRange * limitRange)
		{
			return std::make_shared<Tank::ReturnHome>();// 帰還状態へ
		}

		// 移動処理
		VECTOR vDir = owner->GetDir();
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);	
		owner->SetMove(vMove);

		if (_fTimer >= owner->GetEnemyParam().fMoveTime)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		return nullptr;
	}

	void Tank::Move::UpdateSearch(Enemy* owner)
	{
		if (Tank::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 発見
	void Tank::Detect::Enter(Enemy* owner)
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

	std::shared_ptr<EnemyState> Tank::Detect::Update(Enemy* owner)
	{
		_fTimer++;

		// ターゲットの方向を向く
		auto target = owner->GetTarget();	
		if (target)
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vDir = VNorm(vToTarget);
			owner->SetDir(vDir);
		}

		if (_fTimer >= owner->GetEnemyParam().fDetectTime)
		{
			return std::make_shared<Tank::Chase>();// 攻撃判定へ
		}

		return nullptr;
	}


	// 追跡
	void Tank::Chase::Enter(Enemy* owner)
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

	std::shared_ptr<EnemyState> Tank::Chase::Update(Enemy* owner)
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
		if (dist > owner->GetEnemyParam().fChaseLimitRange) {
			// 索敵結果をクリア
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 攻撃射程内か
		if (dist <= owner->GetEnemyParam().fAttackRange) {
			return std::make_shared<Attack>();// 攻撃状態へ
		}

		// 即座に回転
		VECTOR vDir = VNorm(vToTarget);
		owner->SetDir(vDir);

		// 移動処理
		auto speed = owner->GetEnemyParam().fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);
		owner->SetMove(vMove);// 移動量を更新

		return nullptr;
	}





	// 一段目攻撃前の予備動作
	void Tank::FirstAttackPrepare::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 移動しない

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackPrepare::Update(Enemy* owner)
	{
		_fTimer++;

		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();
		}

		// ターゲットの方向を向き続ける
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		VECTOR vDir = VNorm(vToTarget);
		owner->SetDir(vDir);

		// 予備動作時間経過で一段目攻撃実行へ
		if (_fTimer >= FIRST_PREPARE_TIME)
		{
			return std::make_shared<Tank::FirstAttackExecute>();
		}

		return nullptr;
	}





	// 一段目攻撃の実行
	void Tank::FirstAttackExecute::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bAttackStarted = false;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 攻撃アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);// ループ無し
		}

		// 攻撃コリジョン設定
		owner->StartAttack(FIRST_ATTACK_SETTINGS);	
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackExecute::Update(Enemy* owner)
	{
		_fTimer++;

		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();
		}

		// ターゲットへ接近しながら攻撃
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		auto dist = VSize(vToTarget);

		if (dist > 0.0f)
		{
			// 攻撃発生前までは方向を追従
			if (_fTimer < FIRST_ATTACK_DELAY)
			{
				VECTOR vDir = VNorm(vToTarget);
				owner->SetDir(vDir);
			}

			// 接近移動
			VECTOR vDir = owner->GetDir();
			VECTOR vMove = VScale(vDir, FIRST_APPROACH_SPEED);
			owner->SetMove(vMove);
		}

		// 攻撃判定の位置更新
		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(FIRST_ATTACK_SETTINGS);
		}

		// 攻撃時間経過で後隙へ
		auto totalTime = FIRST_ATTACK_DELAY + FIRST_ATTACK_DURATION + FIRST_ATTACK_RECOVERY;
		if (_fTimer >= totalTime)
		{
			return std::make_shared<Tank::FirstAttackRecovery>();
		}

		return nullptr;
	}

	void Tank::FirstAttackExecute::Exit(Enemy* owner)
	{
		owner->StopAttack();
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 攻撃後は停止
	}





	// 一段目攻撃後隙
	void Tank::FirstAttackRecovery::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bIsCompleted = false;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 移動しない

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackRecovery::Update(Enemy* owner)
	{
		_fTimer++;

		// 硬直時間経過で攻撃予備動作へ
		if (_fTimer >= FIRST_RECOVERY_TIME)
		{
			_bIsCompleted = true;// 後隙完了フラグを立てる
			return std::make_shared<Tank::SecondAttackPrepare>();
		}

		return nullptr;
	}

	void Tank::FirstAttackRecovery::Exit(Enemy* owner)
	{
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 二段目攻撃前の予備動作
	void Tank::SecondAttackPrepare::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 移動しない

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackPrepare::Update(Enemy* owner)
	{
		_fTimer++;

		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();
		}

		// ターゲットの方向を向き続ける
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		VECTOR vDir = VNorm(vToTarget);
		owner->SetDir(vDir);

		// 予備動作時間経過で二段目攻撃実行へ
		if (_fTimer >= SECOND_PREPARE_TIME)
		{
			return std::make_shared<Tank::SecondAttackExecute>();
		}

		return nullptr;
	}





	// 二段目攻撃の実行
	void Tank::SecondAttackExecute::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bAttackStarted = false;

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 攻撃アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);// ループ無し
		}

		// 攻撃コリジョン設定
		owner->StartAttack(SECOND_ATTACK_SETTINGS);
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackExecute::Update(Enemy* owner)
	{
		_fTimer++;

		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();
		}

		// ターゲットへ接近しながら攻撃
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		auto dist = VSize(vToTarget);

		if (dist > 0.0f)
		{
			// 攻撃発生前までは方向を追従
			if (_fTimer < SECOND_ATTACK_DELAY)
			{
				VECTOR vDir = VNorm(vToTarget);
				owner->SetDir(vDir);
			}

			// 接近移動
			VECTOR vDir = owner->GetDir();
			VECTOR vMove = VScale(vDir, SECOND_APPROACH_SPEED);
			owner->SetMove(vMove);
		}

		// 攻撃判定の位置更新
		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(SECOND_ATTACK_SETTINGS);
		}

		// 攻撃時間経過で後隙へ
		auto totalTime = SECOND_ATTACK_DELAY + SECOND_ATTACK_DURATION + SECOND_ATTACK_RECOVERY;
		if (_fTimer >= totalTime)
		{
			return std::make_shared<Tank::SecondAttackRecovery>();
		}

		return nullptr;
	}

	void Tank::SecondAttackExecute::Exit(Enemy* owner)
	{
		owner->StopAttack();
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 攻撃後は停止
	}





	// 二段目攻撃後隙
	void Tank::SecondAttackRecovery::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bIsCompleted = false;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 移動しない

		// ここでアニメーション設定
		// AnimManagerを取得してアニメーション切り替え
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			// 待機アニメーションに切り替え
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);// 無限ループ
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackRecovery::Update(Enemy* owner)
	{
		_fTimer++;

		// 硬直時間経過で回復状態へ
		if (_fTimer >= SECOND_RECOVERY_TIME)
		{
			_bIsCompleted = true;// 後隙完了フラグを立てる
			return std::make_shared<Tank::Chase>();
		}

		return nullptr;
	}

	void Tank::SecondAttackRecovery::Exit(Enemy* owner)
	{
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 攻撃判定ステート
	void Tank::Attack::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
	}

	std::shared_ptr<EnemyState> Tank::Attack::Update(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();
		}

		// 攻撃範囲内なら一段目予備動作へ
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		auto dist = VSize(vToTarget);

		const auto& param = owner->GetEnemyParam();
		if (dist <= param.fAttackRange)
		{
			return std::make_shared<Tank::FirstAttackPrepare>();
		}

		// 範囲外なら接近
		if (dist > 0.0f)
		{
			VECTOR vDir = VNorm(vToTarget);
			owner->SetDir(vDir);

			VECTOR vMove = VScale(vDir, param.fMoveSpeed);
			owner->SetMove(vMove);
		}

		return nullptr;
	}

	void Tank::Attack::Exit(Enemy* owner)
	{
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 停止
	}

	bool Tank::Attack::CanChangeState()
	{
		return true;
	}





	// 帰還
	void Tank::ReturnHome::Enter(Enemy* owner)
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

	std::shared_ptr<EnemyState> Tank::ReturnHome::Update(Enemy* owner)
	{
		_fTimer++;
		// 初期位置方向を向く
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());// 初期位置へのベクトル
		auto dist = VSize(vToHome);// 初期位置までの距離
		if (dist > 0.0f)
		{
			// 即座に方向を変更
			VECTOR vDir = VNorm(vToHome);
			owner->SetDir(vDir);

			// 接近移動
			VECTOR vMove = VScale(vDir, owner->GetEnemyParam().fMoveSpeed);
			owner->SetMove(vMove);
		}
		// 初期位置付近に到達したら待機状態へ
		if (dist <= NEARBY_HOME)
		{
			return std::make_shared<Tank::Idle>();
		}
		return nullptr;
	}

	void Tank::ReturnHome::UpdateSearch(Enemy* owner)
	{
		if (Tank::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}
}