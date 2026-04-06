// 担当者 : 松田

#include "StateTank.h"
#include "../Enemy.h"
#include "StateCommon.h"
#include "../EnemyAttackSettings.h"
#include "EnemyState_impl.h"

namespace 
{
	// 一段目攻撃
	constexpr auto FIRST_PREPARE_TIME = 60.0f;// 攻撃準備時間
	constexpr float FIRST_EXECUTE_TIME = 60.0f;// 攻撃実行時間
	constexpr auto FIRST_RECOVERY_TIME = 30.0f;// 攻撃後の硬直時間
	constexpr auto FIRST_ATTACK_DELAY = 30.0f;// 攻撃発生フレーム
	constexpr auto FIRST_ATTACK_DURATION = 10.0f;// 攻撃持続フレーム
	constexpr auto FIRST_ATTACK_DAMAGE = 8.0f;// ダメージ
	constexpr auto FIRST_EXECUTE_SPEED = 5.0f;// 攻撃実行時の前進速度

	// 二段目攻撃
	constexpr auto SECOND_PREPARE_TIME = 20.0f;// 攻撃準備時間
	constexpr float SECOND_EXECUTE_TIME = 60.0f;// 攻撃実行時間
	constexpr auto SECOND_RECOVERY_TIME = 30.0f;// 攻撃後の硬直時間
	constexpr auto SECOND_ATTACK_DELAY = 30.0f;// 攻撃発生フレーム
	constexpr auto SECOND_ATTACK_DURATION = 10.0f;// 攻撃持続フレーム
	constexpr auto SECOND_ATTACK_DAMAGE = 8.0f;// ダメージ
	constexpr auto SECOND_EXECUTE_SPEED = 5.0f;// 攻撃実行時の前進速度

	// 三段目攻撃
	constexpr auto THIRD_PREPARE_TIME = 20.0f;// 攻撃準備時間
	constexpr float THIRD_EXECUTE_TIME = 60.0f;// 攻撃実行時間
	constexpr auto THIRD_RECOVERY_TIME = 60.0f;// 攻撃後の硬直時間
	constexpr auto THIRD_ATTACK_DELAY = 30.0f;// 攻撃発生フレーム
	constexpr auto THIRD_ATTACK_DURATION = 10.0f;// 攻撃持続フレーム
	constexpr auto THIRD_ATTACK_DAMAGE = 12.0f;// ダメージ
	constexpr auto THIRD_EXECUTE_SPEED = 8.0f;// 攻撃実行時の前進速度

	// 突進攻撃
	constexpr auto CHARGE_PREPARE_TIME = 10.0f;// 突進準備時間
	constexpr auto CHARGE_EXECUTE_TIME = 90.0f;// 突進の最大時間
	constexpr auto CHARGE_RECOVERY_TIME = 60.0f;// 突進後の硬直時間
	constexpr auto CHARGE_SPEED = 10.0f;// 突進速度
	constexpr auto CHARGE_DAMAGE = 5.0f;// 突進ダメージ
	constexpr auto CHARGE_ARRIVE_DIST = 5.0f;// 突進完了とみなす距離

	constexpr float ATTACK_START_RANGE = 650.0f;// 攻撃開始距離
	constexpr auto CHARGE_ATTACK_RANGE = 640.0f;// 突進を試みる距離
	constexpr auto CHARAGE_JUDGE_INTERVAL = 30.0f;// 突進判定を行う間隔(フレーム)
	constexpr auto NORMAL_ATTACK_RANGE = 150.0f;// 通常攻撃を試みる距離
	constexpr auto CHARGE_ATTACK_PROB = 0.4f;// 突進の発生確率(0.0〜1.0)
	constexpr auto THIRD_ATTACK_PROB = 0.8f;// 三段目攻撃の発生確率(0.0〜1.0)

	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;// スムーズ回転速度
	constexpr auto FAST_ROTATE_SPEED = 10.0f;// 高速回転速度

	constexpr auto ATTACK_WAIT_TIME = 10.0f;// 攻撃選択後の待機時間

	// アニメーション設定
	// アニメーション制御用定数
	constexpr auto BLEND_FRAME = 10.0f;// アニメーションブレンドフレーム数
	constexpr auto ANIM_LOOP_COUNT = 0;// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;// アニメーション再生回数
	constexpr auto ANIM_SPEED_HALF = 0.5f;// アニメーション再生速度(半分)

	// 攻撃コリジョン設定
	EnemyAttackSettings MakeFirstAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, 150.0f, 100.0f);// 前方
		settings.vBottomOffset = VGet(0.0f, 50.0f, 100.0f);
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
		settings.vTopOffset = VGet(0.0f, 150.0f, 100.0f);// 前方
		settings.vBottomOffset = VGet(0.0f, 50.0f, 100.0f);
		settings.fRadius = 80.0f;
		settings.fDelay = SECOND_ATTACK_DELAY;
		settings.fDuration = SECOND_ATTACK_DURATION;
		settings.fRecovery = 0.0f;
		settings.fDamage = SECOND_ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	EnemyAttackSettings MakeThirdAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, 150.0f, 100.0f);// 前方
		settings.vBottomOffset = VGet(0.0f, 50.0f, 100.0f);
		settings.fRadius = 90.0f;
		settings.fDelay = THIRD_ATTACK_DELAY;
		settings.fDuration = THIRD_ATTACK_DURATION;
		settings.fRecovery = 0.0f;
		settings.fDamage = THIRD_ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	EnemyAttackSettings MakeChargeAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, 150.0f, 100.0f);// 前方
		settings.vBottomOffset = VGet(0.0f, 50.0f, 100.0f);
		settings.fRadius = 60.0f;
		settings.fDelay = 0.0f;
		settings.fDuration = CHARGE_EXECUTE_TIME;
		settings.fRecovery = 0.0f;
		settings.fDamage = CHARGE_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	// 定数として保存
	const EnemyAttackSettings FIRST_ATTACK_SETTINGS = MakeFirstAttackSettings();
	const EnemyAttackSettings SECOND_ATTACK_SETTINGS = MakeSecondAttackSettings();
	const EnemyAttackSettings THIRD_ATTACK_SETTINGS = MakeThirdAttackSettings();
	const EnemyAttackSettings CHARGE_ATTACK_SETTINGS = MakeChargeAttackSettings();
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
			animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::Idle::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		if (_fTimer >= _fTargetTimer)
		{
			return owner->GetState<Tank::Approach>();
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
			animManager->ChangeAnimationByName("mainRig|Walk01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}

		// 経路情報をリセット
		owner->ClearPath();
	}

	std::shared_ptr<EnemyState> Tank::Approach::Update(Enemy* owner)
	{
		auto targetInfo = GetTargetInfo(owner);
		if (!targetInfo.bExist)
		{
			return owner->GetState<Tank::Idle>();
		}

		const auto& param = owner->GetEnemyParam();

		// 攻撃可能範囲に入ったら攻撃ステートへ
		if (targetInfo.fDist <= ATTACK_START_RANGE)
		{
			// 無限ループ防止のため、ここでも判定
			if (owner->IsVisible(targetInfo.target->GetPos(), 5.0f))
			{
				return owner->GetState<Tank::Attack>();
			}
		}

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
		_fTimer = 0.0f;
		_bJudgedCharge = false;
	}

	std::shared_ptr<EnemyState> Tank::Attack::Update(Enemy* owner)
	{
		auto targetInfo = GetTargetInfo(owner);
		if (!targetInfo.bExist)
		{
			return owner->GetState<Tank::Idle>();
		}

		const auto& param = owner->GetEnemyParam();

		// ターゲットが離れたら接近ステートへ
		if (targetInfo.fDist > ATTACK_START_RANGE)
		{
			return owner->GetState<Tank::Approach>();
		}

		// 壁でターゲットが見えない場合、接近ステートへ
		if (!owner->IsVisible(targetInfo.target->GetPos(), 5.0f))
		{
			return owner->GetState<Tank::Approach>();
		}

		// 通常攻撃距離まで来たら一段目攻撃へ
		if (targetInfo.fDist <= NORMAL_ATTACK_RANGE)
		{
			StopMove(owner);
			return owner->GetState<Tank::FirstAttackPrepare>();
		}

		// 突進可能距離内で一度だけ判定する
		if (targetInfo.fDist <= CHARGE_ATTACK_RANGE && !_bJudgedCharge)
		{
			_bJudgedCharge = true;

			// 突進攻撃を行うかの判定
			if (mymath::RandomRange(0.0f, 1.0f) < CHARGE_ATTACK_PROB)
			{
				StopMove(owner);
				return owner->GetState<Tank::ChargePrepare>(targetInfo.target->GetPos());
			}
		}

		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, targetInfo.vDir, param.fMoveSpeed);

		return nullptr;
	}



	// 一段目攻撃前の予備動作
	void Tank::FirstAttackPrepare::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackPrepare::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		}

		if (_fTimer >= FIRST_PREPARE_TIME)
		{
			return owner->GetState<Tank::FirstAttackExecute>();
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
			animManager->ChangeAnimationByName("mainRig|Attack01", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_HALF);
		}

		// 攻撃コリジョン生成
		owner->StartAttack(FIRST_ATTACK_SETTINGS);	
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackExecute::Update(Enemy* owner)
	{
		_fTimer++;

		// 少し前進しながら攻撃
		if (_fTimer >= FIRST_ATTACK_DELAY && _fTimer <= FIRST_ATTACK_DELAY + FIRST_ATTACK_DURATION)
		{
			MoveToTarget(owner, owner->GetDir(), FIRST_EXECUTE_SPEED);
		}
		else// その後は移動停止
		{
			StopMove(owner);
		}

		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(FIRST_ATTACK_SETTINGS);
		}

		// 攻撃時間経過チェック
		if (_fTimer >= FIRST_EXECUTE_TIME)
		{
			return owner->GetState<Tank::FirstAttackRecovery>();// 一段目後隙へ
		}

		return nullptr;
	}

	void Tank::FirstAttackExecute::Exit(Enemy* owner)
	{
		if (_bAttackStarted)
		{
			owner->StopAttack();
			_bAttackStarted = false;
		}
		StopMove(owner);
	}

	// 一段目攻撃後隙
	void Tank::FirstAttackRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bIsCompleted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::FirstAttackRecovery::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		// 硬直時間経過チェック
		if (_fTimer >= FIRST_RECOVERY_TIME)
		{
			_bIsCompleted = true;
			return owner->GetState<Tank::SecondAttackPrepare>();// 二段目予備動作へ
		}

		return nullptr;
	}



	// 二段目攻撃前の予備動作
	void Tank::SecondAttackPrepare::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackPrepare::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, FAST_ROTATE_SPEED);
		}

		if (_fTimer >= SECOND_PREPARE_TIME)
		{
			return owner->GetState<Tank::SecondAttackExecute>();
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
			animManager->ChangeAnimationByName("mainRig|Attack02", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_HALF);
		}

		// 攻撃コリジョン生成
		owner->StartAttack(SECOND_ATTACK_SETTINGS);
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackExecute::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		if (_fTimer >= SECOND_ATTACK_DELAY && _fTimer <= SECOND_ATTACK_DELAY + SECOND_ATTACK_DURATION)
		{
			MoveToTarget(owner, owner->GetDir(), SECOND_EXECUTE_SPEED);
		}
		else
		{
			StopMove(owner);
		}

		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(SECOND_ATTACK_SETTINGS);
		}

		// 攻撃時間経過チェック
		if (_fTimer >= SECOND_EXECUTE_TIME)
		{
			return owner->GetState<Tank::SecondAttackRecovery>();// 二段目後隙へ
		}

		return nullptr;
	}

	void Tank::SecondAttackExecute::Exit(Enemy* owner)
	{
		if (_bAttackStarted)
		{
			owner->StopAttack();
			_bAttackStarted = false;
		}
		StopMove(owner);
	}

	// 二段目攻撃後隙
	void Tank::SecondAttackRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bIsCompleted = false;
		_bJudged = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::SecondAttackRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		StopMove(owner);

		// 硬直時間経過チェック
		if (_fTimer >= SECOND_RECOVERY_TIME)
		{
			// 三段目にいくか抽選
			if (!_bJudged)
			{
				_bJudged = true;
				_bIsCompleted = true;

				if (mymath::RandomRange(0.0f, 1.0f) < THIRD_ATTACK_PROB)
				{
					return owner->GetState<Tank::ThirdAttackPrepare>();
				}

				return owner->GetState<Tank::Idle>();
			}
		}

		return nullptr;
	}



	// 三段目攻撃前の予備動作
	void Tank::ThirdAttackPrepare::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::ThirdAttackPrepare::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, FAST_ROTATE_SPEED);
		}

		if (_fTimer >= THIRD_PREPARE_TIME)
		{
			return owner->GetState<Tank::ThirdAttackExecute>();
		}

		return nullptr;
	}

	// 三段目攻撃の実行
	void Tank::ThirdAttackExecute::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bAttackStarted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("mainRig|Attack04", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_HALF);
		}

		// 攻撃コリジョン生成
		 owner->StartAttack(THIRD_ATTACK_SETTINGS);
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::ThirdAttackExecute::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		if (_fTimer >= THIRD_ATTACK_DELAY && _fTimer <= THIRD_ATTACK_DELAY + THIRD_ATTACK_DURATION)
		{
			MoveToTarget(owner, owner->GetDir(), THIRD_EXECUTE_SPEED);
		}
		else
		{
			StopMove(owner);
		}

		if (_bAttackStarted)
		{
			 owner->UpdateAttackTransform(THIRD_ATTACK_SETTINGS);
		}

		if(_fTimer >= THIRD_EXECUTE_TIME)
		{
			return owner->GetState<Tank::ThirdAttackRecovery>();
		}

		return nullptr;
	}

	void Tank::ThirdAttackExecute::Exit(Enemy* owner)
	{
		if (_bAttackStarted)
		{
			 owner->StopAttack();
			_bAttackStarted = false;
		}
		StopMove(owner);
	}

	// 三段目攻撃後隙
	void Tank::ThirdAttackRecovery::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bIsCompleted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::ThirdAttackRecovery::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		if (_fTimer >= THIRD_RECOVERY_TIME)
		{
			_bIsCompleted = true;
			return owner->GetState<Tank::Idle>();
		}

		return nullptr;
	}



	// 突進前の予備動作
	void Tank::ChargePrepare::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			//animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::ChargePrepare::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		// 予備動作中はターゲットの方向を向く
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, FAST_ROTATE_SPEED);
		}


		if(_fTimer >= CHARGE_PREPARE_TIME)
		{
			// 正規化した方向ベクトル、突進先の座標を渡す
			VECTOR vToTarget = VSub(_vChargeTarget, owner->GetPos());
			vToTarget.y = 0.0f;
			VECTOR vDir = VSquareSize(vToTarget) > 0.0001f
				? mymath::FlattenVector(vToTarget)
				: owner->GetDir();

			return owner->GetState<Tank::ChargeExecute>(vDir, _vChargeTarget);
		}

		return nullptr;
	}

	// 突進の実行
	void Tank::ChargeExecute::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bAttackStarted = false;

		owner->SetDir(_vChargeDir);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("mainRig|Run01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}

		// 攻撃コリジョン生成
		owner->StartAttack(CHARGE_ATTACK_SETTINGS);
		_bAttackStarted = true;
	}

	std::shared_ptr<EnemyState> Tank::ChargeExecute::Update(Enemy* owner)
	{
		_fTimer++;

		if (_bAttackStarted)
		{
			owner->UpdateAttackTransform(CHARGE_ATTACK_SETTINGS);
		}

		// 突進先の座標への水平距離
		VECTOR vToTarget = VSub(_vChargeTarget, owner->GetPos());
		vToTarget.y = 0.0f;
		float dist = VSize(vToTarget);

		if (dist <= CHARGE_ARRIVE_DIST)
		{
			return owner->GetState<Tank::ChargeRecovery>();
		}

		MoveToTarget(owner, _vChargeDir, CHARGE_SPEED);

		if (_fTimer >= CHARGE_EXECUTE_TIME)
		{
			return owner->GetState<Tank::ChargeRecovery>();
		}

		return nullptr;
	}

	void Tank::ChargeExecute::Exit(Enemy* owner)
	{
		if (_bAttackStarted)
		{
			owner->StopAttack();
			_bAttackStarted = false;
		}
		StopMove(owner);
	}

	// 突進後の後隙
	void Tank::ChargeRecovery::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_bIsCompleted = false;

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("mainRig|Idle01", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Tank::ChargeRecovery::Update(Enemy* owner)
	{
		_fTimer++;

		StopMove(owner);

		if(_fTimer >= CHARGE_RECOVERY_TIME)
		{
			_bIsCompleted = true;
			return owner->GetState<Tank::Idle>();
		}

		return nullptr;
	}
}