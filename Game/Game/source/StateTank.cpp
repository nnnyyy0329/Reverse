#include "StateTank.h"
#include "Enemy.h"
#include "StateCommon.h"
#include "EnemyAttackSettings.h"
#include "EnemyState_impl.h"

namespace 
{
	// 一段目攻撃
	constexpr auto FIRST_PREPARE_TIME = 40.0f;// 予備動作時間
	constexpr float FIRST_EXECUTE_TIME = 60.0f;// 攻撃実行時間
	constexpr auto FIRST_RECOVERY_TIME = 25.0f;// 攻撃後の硬直時間

	constexpr auto FIRST_ATTACK_DELAY = 5.0f;// 攻撃発生フレーム
	constexpr auto FIRST_ATTACK_DURATION = 20.0f;// 攻撃持続フレーム
	constexpr auto FIRST_ATTACK_DAMAGE = 1.0f;// ダメージ

	constexpr auto FIRST_APPROACH_SPEED = 4.0f;// 接近速度

	// 二段目攻撃
	constexpr auto SECOND_PREPARE_TIME = 35.0f;// 予備動作時間
	constexpr float SECOND_EXECUTE_TIME = 60.0f;// 攻撃実行時間
	constexpr auto SECOND_RECOVERY_TIME = 120.0f;// 攻撃後の硬直時間

	constexpr auto SECOND_ATTACK_DELAY = 8.0f;// 攻撃発生フレーム
	constexpr auto SECOND_ATTACK_DURATION = 25.0f;// 攻撃持続フレーム
	constexpr auto SECOND_ATTACK_DAMAGE = 1.0f;// ダメージ

	constexpr auto SECOND_APPROACH_SPEED = 5.0f;// 接近速度

	// アニメーション設定
	constexpr auto BLEND_FRAME = 1.0f;// アニメーションブレンドフレーム数

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
		settings.fRecovery = 0.0f;
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
		settings.fRecovery = 0.0f;
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
	// 待機
	void Tank::Idle::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fIdleTime, 0.0f);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_00", BLEND_FRAME, 0);
		}
	}

	std::shared_ptr<EnemyState> Tank::Idle::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			return std::make_shared<Tank::Approach>();
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
			animManager->ChangeAnimationByName("Nenemy_walk_01", BLEND_FRAME, 0);
		}

		// 経路情報をリセット
		owner->ClearPath();
	}

	std::shared_ptr<EnemyState> Tank::Approach::Update(Enemy* owner)
	{
		auto targetInfo = GetTargetInfo(owner);
		if (!targetInfo.bExist)
		{
			return std::make_shared<Tank::Idle>();
		}

		const auto& param = owner->GetEnemyParam();

		// 攻撃可能範囲に入ったら攻撃ステートへ
		//if (targetInfo.fDist <= param.fAttackRange)
		//{
		//	return std::make_shared<Tank::Attack>();
		//}

		// A*経路探索を使った移動
		// 1.ターゲットの現在位置を渡して、ルートを更新
		owner->UpdatePath(targetInfo.target->GetPos());

		// 2.ルート上の次に向かうべきウェイポイントの座標を取得
		VECTOR vNextWpPos = owner->GetNextWaypoint();

		// 3.現在地から次のウェイポイントへのベクトルを計算
		VECTOR vToWp = VSub(vNextWpPos, owner->GetPos());
		vToWp.y = 0.0f;// Y成分を除去

		// 4.ルートが存在すれば移動を行う
		if (VSquareSize(vToWp) > 0.0001f)
		{
			// ウェイポイントへ方向ベクトルを正規化
			VECTOR vMoveDir = mymath::FlattenVector(vToWp);

			// 移動・旋回
			VECTOR vMove = VScale(vMoveDir, param.fMoveSpeed);
			owner->SetMove(vMove);
			owner->SmoothRotateTo(vMoveDir, param.fTurnSpeed);
		}
		else
		{
			// ルートが見つからない、またはゴールに到達している場合
			VECTOR vDir = targetInfo.vDir;
			StopMove(owner);
			owner->SmoothRotateTo(vDir, param.fTurnSpeed);
		}

		return nullptr;
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
		if (_fTimer >= FIRST_EXECUTE_TIME)
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
		if (_fTimer >= SECOND_EXECUTE_TIME)
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
}