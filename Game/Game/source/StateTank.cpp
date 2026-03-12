#include "StateTank.h"
#include "Enemy.h"
#include "StateCommon.h"
#include "EnemyAttackSettings.h"
#include "EnemyState_impl.h"

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

	constexpr auto LOST_NEARBY_HOME = 10.0f;// 帰還完了判定距離

	constexpr auto LOST_ROTATE_SPEED = 3.0f;// 見渡し回転速度

	constexpr auto LOST_LOOK_MIN_TIME = 60.0f;// 方向あたりの最低見渡し時間

	// 徘徊制御
	constexpr auto WANDER_RANDOM_ANGLE_RANGE = 90;// 徘徊時のランダム角度範囲(度)
	constexpr auto WANDER_RANDOM_ANGLE_OFFSET = 45;// 徘徊時のランダム角度オフセット(度)

	// 時間ランダム幅定数
	constexpr auto IDLE_TIME_RANGE = 30.0f;// 待機時間のランダム幅
	constexpr auto NOTICE_TIME_RANGE = 10.0f;// 発見硬直のランダム幅
	constexpr auto LOST_LOOK_RANDOM_TIME = 60.0f;// 見渡し時間ランダム幅

	constexpr auto LOST_LOOK_COUNT = 3;// 見渡し回数

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

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fIdleTime, IDLE_TIME_RANGE);

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

		// ランダム方向へ向かう
		float targetAngle = static_cast<float>(GetRand(359)) * DEGREE_TO_RADIAN;
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

		// 移動可能範囲外チェック
		auto areaResult = TransitionToIdleOutsideArea<Idle>(owner);
		if (areaResult) { return areaResult; }

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

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fDetectTime, NOTICE_TIME_RANGE);

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
		if (_fTimer >= _fTargetTimer)
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

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

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





	// ターゲットを見失ったとき
	void LostTarget::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_ePhase = Phase::LOOK_AROUND;// 見渡しから
		_lookCnt = 0;

		// 見渡し方向
		float fCurrentAngle = atan2f(owner->GetDir().x, owner->GetDir().z);
		float fOffset = static_cast<float>(GetRand(180) - 90) * DEGREE_TO_RADIAN;// -90 ~ +90度のランダムオフセット 
		float fTargetAngle = fCurrentAngle + fOffset;
		_vLookDir = VGet(sinf(fTargetAngle), 0.0f, cosf(fTargetAngle));
		_vLookDir = VNorm(_vLookDir);
		// 見渡し時間
		_fLookDuration = CalcRandomRangeTime(LOST_LOOK_MIN_TIME, LOST_LOOK_RANDOM_TIME);
		_fLookTimer = 0.0f;

		StopMove(owner);

		// アニメーション設定
	}

	std::shared_ptr<EnemyState> LostTarget::Update(Enemy* owner)
	{
		// 再索敵
		if (owner->IsTargetDetected())
		{
			return std::make_shared<Notice>();
		}

		StopMove(owner);

		if (_ePhase == Phase::LOOK_AROUND)
		{
			// 目標方向へ回転
			RotateToTarget(owner, _vLookDir, LOST_ROTATE_SPEED);

			_fLookTimer++;

			// 見渡し時間経過で次の見渡し方向へ
			if (_fLookTimer >= _fLookDuration)
			{
				_lookCnt++;

				if (_lookCnt >= LOST_LOOK_COUNT)
				{
					// 見渡し完了で帰還フェーズで
					_ePhase = Phase::RETURN_HOME;

					// アニメーション設定
				}
			}
			else
			{
				// 次の見渡し方向をランダムに決定
				float fCurrentAngle = atan2f(owner->GetDir().x, owner->GetDir().z);
				float fOffset = static_cast<float>(GetRand(240) - 120) * DEGREE_TO_RADIAN;// -120 ~ +120度のランダムオフセット
				float fTargetAngle = fCurrentAngle + fOffset;
				_vLookDir = VGet(sinf(fTargetAngle), 0.0f, cosf(fTargetAngle));
				_vLookDir = VNorm(_vLookDir);
				// 見渡し時間
				_fLookDuration = CalcRandomRangeTime(LOST_LOOK_MIN_TIME, LOST_LOOK_RANDOM_TIME);
				_fLookTimer = 0.0f;
			}
		}
		else// RETURN_HOME
		{
			// 初期位置への距離計算
			VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
			float dist = VSize(vToHome);

			if (dist <= LOST_NEARBY_HOME)
			{
				return std::make_shared<Idle>();// 帰還完了
			}

			// 初期位置方向へ移動
			VECTOR vDir = VNorm(vToHome);
			RotateToTarget(owner, vDir, LOST_ROTATE_SPEED);
			MoveToTarget(owner, vDir, owner->GetEnemyParam().fMoveSpeed);
		}

		return nullptr;
	}

	void LostTarget::UpdateSearch(Enemy* owner)
	{
		owner->SetTargetDetected(Tank::IsTargetVisible(owner));
	}

}