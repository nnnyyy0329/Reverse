#include "StateTank.h"
#include "Enemy.h"
#include "StateCommon.h"
#include "EnemyAttackSettings.h"

namespace 
{
	// 一段目攻撃タイミング
	constexpr auto FIRST_PREPARE_TIME = 40.0f;// 予備動作時間
	constexpr auto FIRST_ATTACK_DELAY = 5.0f;// 攻撃発生フレーム
	constexpr auto FIRST_ATTACK_DURATION = 20.0f;// 攻撃持続フレーム
	constexpr auto FIRST_ATTACK_RECOVERY = 10.0f;// 後隙フレーム
	constexpr auto FIRST_RECOVERY_TIME = 25.0f;// 攻撃後の硬直時間
	constexpr auto FIRST_ATTACK_DAMAGE = 1.0f;// ダメージ
	constexpr auto FIRST_APPROACH_SPEED = 4.0f;// 接近速度

	// 二段目攻撃タイミング
	constexpr auto SECOND_PREPARE_TIME = 35.0f;// 予備動作時間	
	constexpr auto SECOND_ATTACK_DELAY = 8.0f;// 攻撃発生フレーム
	constexpr auto SECOND_ATTACK_DURATION = 25.0f;// 攻撃持続フレーム
	constexpr auto SECOND_ATTACK_RECOVERY = 15.0f;// 後隙フレーム
	constexpr auto SECOND_RECOVERY_TIME = 120.0f;// 攻撃後の硬直時間	
	constexpr auto SECOND_ATTACK_DAMAGE = 1.0f;// ダメージ
	constexpr auto SECOND_APPROACH_SPEED = 5.0f;// 接近速度

	// アニメーション設定
	constexpr auto BLEND_FRAME = 1.0f;// アニメーションブレンドフレーム数

	// 徘徊制御
	constexpr auto WANDER_HOME_RETURN_RATIO = 0.8f;// 初期位置へ戻る判定距離係数
	constexpr auto WANDER_RANDOM_ANGLE_RANGE = 90;// 徘徊時のランダム角度範囲(度)
	constexpr auto WANDER_RANDOM_ANGLE_OFFSET = 45;// 徘徊時のランダム角度オフセット(度)
	constexpr auto WANDER_RANDOM_ANGLE_MAX = 359;// 徘徊時の完全ランダム角度最大値(度)

	// 帰還制御
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

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);

		// 距離チェック
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 障害物チェック

		return true;// 視界内
	}





	// 待機
	void Tank::Idle::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fTargetTimer = CalcOffsetTime(owner, owner->GetEnemyParam().fIdleTime);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::Idle::Update(Enemy* owner)
	{
		// ターゲット発見チェック
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Tank::Notice>();// 発見状態へ
		}

		// タイマー更新
		_fTimer++;

		// 待機時間経過チェック
		if (_fTimer >= _fTargetTimer)
		{
			return std::make_shared<Tank::Wander>();// 徘徊状態へ
		}

		return nullptr;
	}

	void Tank::Idle::UpdateSearch(Enemy* owner)
	{
		// ターゲット視界チェック
		if (Tank::IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 徘徊
	void Tank::Wander::Enter(Enemy* owner)
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
		float dist = VSize(vToHome);
		float limitRange = owner->GetEnemyParam().fMoveRadius;

		float targetAngle = 0.0f;

		if (dist > limitRange * WANDER_HOME_RETURN_RATIO)
		{
			// 初期位置方向計算
			float toHomeAngle = atan2f(vToHome.z, vToHome.x);
			float randOffset = static_cast<float>(GetRand(WANDER_RANDOM_ANGLE_RANGE) - WANDER_RANDOM_ANGLE_OFFSET) * DEGREE_TO_RADIAN;
			targetAngle = toHomeAngle + randOffset;
		}
		else
		{
			// ランダム方向計算
			targetAngle = static_cast<float>(GetRand(WANDER_RANDOM_ANGLE_MAX)) * DEGREE_TO_RADIAN;
		}

		// 方向ベクトル設定
		VECTOR vDir = VGet(cosf(targetAngle), 0.0f, sinf(targetAngle));	
		owner->SetDir(vDir);
	}

	std::shared_ptr<EnemyState> Tank::Wander::Update(Enemy* owner)
	{
		// ターゲット発見チェック
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Tank::Notice>();// 発見状態へ
		}

		// タイマー更新
		_fTimer++;

		// パラメータ取得
		const auto& param = owner->GetEnemyParam();

		// 移動範囲チェック
		VECTOR vFromHome = VSub(owner->GetPos(), owner->GetHomePos());
		float distSq = VSquareSize(vFromHome);
		float limitRange = param.fMoveRadius;

		if (distSq > limitRange * limitRange)
		{
			return std::make_shared<Tank::ReturnHome>();// 帰還状態へ
		}

		// 移動処理
		VECTOR vDir = owner->GetDir();
		float speed = param.fMoveSpeed;
		VECTOR vMove = VScale(vDir, speed);	
		owner->SetMove(vMove);

		// 移動時間経過チェック
		if (_fTimer >= param.fMoveTime)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		return nullptr;
	}

	void Tank::Wander::UpdateSearch(Enemy* owner)
	{
		// ターゲット視界チェック
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
	void Tank::Notice::Enter(Enemy* owner)
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

	std::shared_ptr<EnemyState> Tank::Notice::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ターゲット方向計算
		auto target = owner->GetTarget();	
		if (target)
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vToTargetNorm = VNorm(vToTarget);
			owner->SetDir(vToTargetNorm);
		}

		// 発見時間経過チェック
		if (_fTimer >= owner->GetEnemyParam().fDetectTime)
		{
			return std::make_shared<Tank::Approach>();// 接近状態へ
		}

		return nullptr;
	}





	// 接近
	void Tank::Approach::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::Approach::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// パラメータ取得
		const auto& param = owner->GetEnemyParam();

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);

		// 追跡範囲チェック
		if (dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 攻撃範囲チェック
		if (dist <= param.fAttackRange)
		{
			return std::make_shared<Attack>();// 攻撃状態へ
		}

		// 回転処理
		VECTOR vToTargetNorm = VNorm(vToTarget);
		owner->SetDir(vToTargetNorm);

		// 移動処理
		float speed = param.fMoveSpeed;
		VECTOR vMove = VScale(vToTargetNorm, speed);
		owner->SetMove(vMove);

		return nullptr;
	}





	// 一段目攻撃前の予備動作
	void Tank::FirstAttackPrepare::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackPrepare::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// タイマー更新
		_fTimer++;

		// ターゲット方向計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		VECTOR vToTargetNorm = VNorm(vToTarget);

		// 回転処理
		owner->SetDir(vToTargetNorm);

		// 予備動作時間経過チェック
		if (_fTimer >= FIRST_PREPARE_TIME)
		{
			return std::make_shared<Tank::FirstAttackExecute>();// 一段目攻撃実行へ
		}

		return nullptr;
	}





	// 一段目攻撃の実行
	void Tank::FirstAttackExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bAttackStarted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);
		}

		// 攻撃開始処理
		owner->StartAttack(FIRST_ATTACK_SETTINGS);	
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackExecute::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// タイマー更新
		_fTimer++;

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		float dist = VSize(vToTarget);

		if (dist > 0.0f)
		{
			// 回転処理
			if (_fTimer < FIRST_ATTACK_DELAY)
			{
				VECTOR vToTargetNorm = VNorm(vToTarget);
				owner->SetDir(vToTargetNorm);
			}

			// 移動処理
			VECTOR vDir = owner->GetDir();
			VECTOR vMove = VScale(vDir, FIRST_APPROACH_SPEED);
			owner->SetMove(vMove);
		}

		// 攻撃判定更新処理
		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(FIRST_ATTACK_SETTINGS);
		}

		// 攻撃時間経過チェック
		float totalTime = FIRST_ATTACK_DELAY + FIRST_ATTACK_DURATION + FIRST_ATTACK_RECOVERY;
		if (_fTimer >= totalTime)
		{
			return std::make_shared<Tank::FirstAttackRecovery>();// 一段目後隙へ
		}

		return nullptr;
	}

	void Tank::FirstAttackExecute::Exit(Enemy* owner)
	{
		// 攻撃終了処理
		owner->StopAttack();
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 一段目攻撃後隙
	void Tank::FirstAttackRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bIsCompleted = false;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 硬直時間経過チェック
		if (_fTimer >= FIRST_RECOVERY_TIME)
		{
			_bIsCompleted = true;
			return std::make_shared<Tank::SecondAttackPrepare>();// 二段目予備動作へ
		}

		return nullptr;
	}

	void Tank::FirstAttackRecovery::Exit(Enemy* owner)
	{
		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 二段目攻撃前の予備動作
	void Tank::SecondAttackPrepare::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackPrepare::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// タイマー更新
		_fTimer++;

		// ターゲット方向計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		VECTOR vToTargetNorm = VNorm(vToTarget);

		// 回転処理
		owner->SetDir(vToTargetNorm);

		// 予備動作時間経過チェック
		if (_fTimer >= SECOND_PREPARE_TIME)
		{
			return std::make_shared<Tank::SecondAttackExecute>();// 二段目攻撃実行へ
		}

		return nullptr;
	}





	// 二段目攻撃の実行
	void Tank::SecondAttackExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bAttackStarted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, 1, 0.5f);
		}

		// 攻撃開始処理
		owner->StartAttack(SECOND_ATTACK_SETTINGS);
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackExecute::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// タイマー更新
		_fTimer++;

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		float dist = VSize(vToTarget);

		if (dist > 0.0f)
		{
			// 回転処理
			if (_fTimer < SECOND_ATTACK_DELAY)
			{
				VECTOR vToTargetNorm = VNorm(vToTarget);
				owner->SetDir(vToTargetNorm);
			}

			// 移動処理
			VECTOR vDir = owner->GetDir();
			VECTOR vMove = VScale(vDir, SECOND_APPROACH_SPEED);
			owner->SetMove(vMove);
		}

		// 攻撃判定更新処理
		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(SECOND_ATTACK_SETTINGS);
		}

		// 攻撃時間経過チェック
		float totalTime = SECOND_ATTACK_DELAY + SECOND_ATTACK_DURATION + SECOND_ATTACK_RECOVERY;
		if (_fTimer >= totalTime)
		{
			return std::make_shared<Tank::SecondAttackRecovery>();// 二段目後隙へ
		}

		return nullptr;
	}

	void Tank::SecondAttackExecute::Exit(Enemy* owner)
	{
		// 攻撃終了処理
		owner->StopAttack();
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 二段目攻撃後隙
	void Tank::SecondAttackRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bIsCompleted = false;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 硬直時間経過チェック
		if (_fTimer >= SECOND_RECOVERY_TIME)
		{
			_bIsCompleted = true;
			return std::make_shared<Tank::Approach>();// 接近状態へ
		}

		return nullptr;
	}

	void Tank::SecondAttackRecovery::Exit(Enemy* owner)
	{
		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 攻撃判定ステート
	void Tank::Attack::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
	}

	std::shared_ptr<EnemyState> Tank::Attack::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// パラメータ取得
		const auto& param = owner->GetEnemyParam();

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		vToTarget.y = 0.0f;
		float dist = VSize(vToTarget);

		// 攻撃範囲チェック
		if (dist <= param.fAttackRange)
		{
			return std::make_shared<Tank::FirstAttackPrepare>();// 一段目予備動作へ
		}

		// 接近処理
		if (dist > 0.0f)
		{
			// 回転処理
			VECTOR vToTargetNorm = VNorm(vToTarget);
			owner->SetDir(vToTargetNorm);

			// 移動処理
			VECTOR vMove = VScale(vToTargetNorm, param.fMoveSpeed);
			owner->SetMove(vMove);
		}

		return nullptr;
	}

	void Tank::Attack::Exit(Enemy* owner)
	{
		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
	}





	// 帰還
	void Tank::ReturnHome::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::ReturnHome::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 初期位置へのベクトル計算
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
		float dist = VSize(vToHome);

		// 帰還到達チェック
		if (dist <= NEARBY_HOME)
		{
			return std::make_shared<Tank::Idle>();// 待機状態へ
		}

		// 回転処理
		if (dist > 0.0f)
		{
			VECTOR vToHomeNorm = VNorm(vToHome);
			owner->SetDir(vToHomeNorm);

			// 移動処理
			VECTOR vMove = VScale(vToHomeNorm, owner->GetEnemyParam().fMoveSpeed);
			owner->SetMove(vMove);
		}

		return nullptr;
	}

	void Tank::ReturnHome::UpdateSearch(Enemy* owner)
	{
		// ターゲット視界チェック
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