#include "StateNormal.h"
#include "Enemy.h"
#include "EnemyState_impl.h"

namespace
{
	// 攻撃コリジョン設定用定数
	constexpr auto ATTACK_COLLISION_OFFSET_Z = 80.0f;	// 攻撃コリジョン前方オフセット
	constexpr auto ATTACK_COLLISION_OFFSET_Y = 60.0f;	// 攻撃コリジョンY位置オフセット
	constexpr auto ATTACK_COLLISION_HEIGHT = 60.0f;		// 攻撃コリジョン高さ
	constexpr auto ATTACK_COLLISION_RADIUS = 40.0f;		// 攻撃コリジョン半径
	constexpr auto ATTACK_DELAY = 10.0f;				// 攻撃発生遅延
	constexpr auto ATTACK_DURATION = 15.0f;				// 攻撃持続時間
	constexpr auto ATTACK_DAMAGE = 20.0f;				// 攻撃ダメージ量

	// 距離判定用定数
	constexpr auto ATTACK_START_DISTANCE = 200.0f;		// 攻撃開始距離
	constexpr auto ATTACK_EXECUTE_DISTANCE = 100.0f;	// 攻撃実行可能距離
	constexpr auto LOST_NEARBY_HOME = 10.0f;			// 帰還完了判定距離

	// 時間制御用定数
	constexpr auto ATTACK_CHARGE_TIME = 40.0f;			// 攻撃溜め時間
	constexpr auto ATTACK_EXECUTE_TIME = 90.0f;			// 攻撃実行時間
	constexpr auto ATTACK_RECOVERY_TIME = 60.0f;		// 攻撃後隙時間
	constexpr auto LOST_WAIT_TIME = 60.0f;				// 帰還前の待機時間

	// 速度制御用定数
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度
	constexpr auto ATTACK_APPROACH_SPEED = 2.0f;		// 攻撃開始時の接近速度
	constexpr auto ATTACK_EXECUTE_SPEED = 2.5f;			// 攻撃実行時の前進速度
	constexpr auto LOST_ROTATE_SPEED = 1.5f;			// 見渡し回転速度

	// 時間ランダム幅定数
	constexpr auto IDLE_TIME_RANGE = 30.0f;				// 待機時間のランダム幅
	constexpr auto WANDER_TIME_RANGE = 30.0f;			// 徘徊時間のランダム幅
	constexpr auto NOTICE_TIME_RANGE = 10.0f;			// 発見硬直のランダム幅
	constexpr auto LOST_LOOK_RANDOM_TIME = 60.0f;		// 見渡し時間ランダム幅

	// 見渡し関連
	constexpr auto LOST_LOOK_ARRIVE_THRESHOLD = 2.0f;	// 見渡し到達判定角度
	constexpr auto LOST_LOOK_ANGLE = 45.0f;				// 見渡し回転角度
	constexpr auto LOST_LOOK_COUNT = 3;					// 見渡し回数

	// 視界チェック用定数
	constexpr auto FAN_VISON_HALF_ANGLE = 45.0f;// 扇形視界の半角(度)
	const float FAN_VISION_COS = cosf(FAN_VISON_HALF_ANGLE * DEGREE_TO_RADIAN);// cos値
	constexpr auto TARGET_DETECT_RADIUS = 20.0f;// ターゲット検出に幅を持たせる(半径)

	// アニメーション制御用定数
	constexpr auto BLEND_FRAME = 10.0f;					// アニメーションブレンドフレーム数
	constexpr auto ANIM_LOOP_COUNT = 0;					// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;					// アニメーション再生回数
	constexpr auto ANIM_SPEED_NORMAL = 1.0f;			// アニメーション再生速度(通常)

	// エフェクト関連
	constexpr auto ATTACK_EFFECT_OFFSET_Y = 80.0f;		// 攻撃エフェクトYオフセット

	// 攻撃コリジョン設定生成
	EnemyAttackSettings MakeNormalAttackSettings()
	{
		EnemyAttackSettings settings;
		settings.colType = COLLISION_TYPE::CAPSULE;
		settings.vTopOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y + ATTACK_COLLISION_HEIGHT, ATTACK_COLLISION_OFFSET_Z);
		settings.vBottomOffset = VGet(0.0f, ATTACK_COLLISION_OFFSET_Y, ATTACK_COLLISION_OFFSET_Z);
		settings.fRadius = ATTACK_COLLISION_RADIUS;
		settings.fDelay = ATTACK_DELAY;
		settings.fDuration = ATTACK_DURATION;
		settings.fRecovery = 0.0f;
		settings.fDamage = ATTACK_DAMAGE;
		settings.ownerId = 0;
		return settings;
	}

	// 定数として保存
	const EnemyAttackSettings NORMAL_ATTACK_SETTINGS = MakeNormalAttackSettings();
}

namespace Normal
{
	// 扇形視界判定
	bool IsTargetVisibleFan(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// Y成分を除去して水平方向ベクトルを取得
		VECTOR vToTarget = mymath::FlattenVector(VSub(target->GetPos(), owner->GetPos()));

		// FlattenVectorはゼロベクトルを返す可能性があるため元の距離で判定
		VECTOR vRaw = VSub(target->GetPos(), owner->GetPos());
		vRaw.y = 0.0f;
		float dist = VSize(vRaw);

		if (dist <= TARGET_DETECT_RADIUS) { return true; }// 発見したとみなす
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 角度チェック: 敵の向きも水平化して正規化
		VECTOR vOwnerDir = mymath::FlattenVector(owner->GetDir());

		float dot = VDot(vOwnerDir, vToTarget);
		if (dot < FAN_VISION_COS) { return false; }

		// 障害物チェック
		VECTOR vStart = owner->GetPos();
		VECTOR vEnd = target->GetPos();
		if (!owner->CheckLineOfSight(vStart, vEnd)) { return false; }

		return true;
	}

	// 待機
	void Idle::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fIdleTime, IDLE_TIME_RANGE);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_01", BLEND_FRAME, ANIM_LOOP_COUNT);
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
		if (_fTimer >= _fTargetTimer)
		{
			return std::make_shared<Wander>();
		}

		return nullptr;
	}

	void Idle::UpdateSearch(Enemy* owner)
	{
		// 扇形判定
		owner->SetTargetDetected(Normal::IsTargetVisibleFan(owner));
	}

	// 徘徊
	void Wander::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fMoveTime, WANDER_TIME_RANGE);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}

		// ランダム方向へ向かう
		float fTargetAngle = mymath::RandomRange(0.0f, DX_TWO_PI_F);
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

		// 移動可能範囲外チェック
		auto areaResult = TransitionToIdleOutsideArea<Idle>(owner);
		if (areaResult) { return areaResult; }

		// タイマー更新
		_fTimer++;

		// 移動処理
		MoveToTarget(owner, owner->GetDir(), owner->GetEnemyParam().fMoveSpeed);

		// 時間経過チェック
		if (_fTimer >= _fTargetTimer)
		{
			return std::make_shared<Idle>();
		}

		return nullptr;
	}

	void Wander::UpdateSearch(Enemy* owner)
	{
		owner->SetTargetDetected(Normal::IsTargetVisibleFan(owner));
	}

	// 発見
	void Notice::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fDetectTime, NOTICE_TIME_RANGE);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> Notice::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ターゲット方向へ回転
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			owner->SetDir(targetInfo.vDir);
		}

		// 時間経過チェック
		if (_fTimer >= _fTargetTimer)
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

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_walk_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> Approach::Update(Enemy* owner)
	{
		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return TransitionToLostNoTarget<LostTarget>(owner);
		}

		// 追跡限界距離チェック
		auto result = TransitionToLostOverChaseLimit<LostTarget>(owner, targetInfo.fDist);
		if (result) { return result; }

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

		// 攻撃開始距離チェック
		if (targetInfo.fDist <= ATTACK_START_DISTANCE)
		{
			return std::make_shared<AttackStart>();
		}

		// ターゲット方向へ回転・移動
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, targetInfo.vDir, owner->GetEnemyParam().fMoveSpeed);

		return nullptr;
	}

	// 攻撃ステート開始
	void AttackStart::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// アニメーション設定
	}

	std::shared_ptr<EnemyState> AttackStart::Update(Enemy* owner)
	{
		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return TransitionToLostNoTarget<LostTarget>(owner);
		}

		// タイマー更新
		_fTimer++;

		// 追跡限界距離チェック
		auto result = TransitionToLostOverChaseLimit<LostTarget>(owner, targetInfo.fDist);
		if (result) { return result; }

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

		if (targetInfo.fDist <= ATTACK_EXECUTE_DISTANCE)
		{
			return std::make_shared<AttackCharge>();
		}

		// ターゲット方向へ回転・移動
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, targetInfo.vDir, ATTACK_APPROACH_SPEED);

		return nullptr;
	}

	// 攻撃溜め
	void AttackCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_NORMAL);
		}
	}

	std::shared_ptr<EnemyState> AttackCharge::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		StopMove(owner);

		// ターゲット方向へ追従
		auto targetInfo = GetTargetInfo(owner);
		if (targetInfo.bExist)
		{
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
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

		// エフェクト
		{
			VECTOR efPos = VAdd(owner->GetPos(), VGet(0.0f, ATTACK_EFFECT_OFFSET_Y, 0.0f));
			auto handle = EffectServer::GetInstance()->Play("Melee_Attack", efPos);

			VECTOR dir = owner->GetDir();
			float rotY = atan2f(dir.x, dir.z);
			VECTOR rotation = VGet(0.0f, rotY, 0.0f);
			EffectServer::GetInstance()->SetRot(handle, rotation);
		}

		// SE
		SoundServer::GetInstance()->Play("SE_Melee_Attanck", DX_PLAYTYPE_BACK);

		// 攻撃コリジョン生成
		owner->StartAttack(NORMAL_ATTACK_SETTINGS);
		_bHasCollision = true;
	}

	std::shared_ptr<EnemyState> AttackExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 少し前進しながら攻撃
		if (_fTimer >= ATTACK_DELAY && _fTimer <= ATTACK_DELAY + ATTACK_DURATION)
		{
			MoveToTarget(owner, owner->GetDir(), ATTACK_EXECUTE_SPEED);
		}
		else// その後は移動停止
		{
			StopMove(owner);
		}

		// 攻撃コリジョン生成
		if (_bHasCollision)
		{
			owner->UpdateAttackTransform(NORMAL_ATTACK_SETTINGS);
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
			animManager->ChangeAnimationByName("enemy_idle_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> AttackRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 移動停止
		StopMove(owner);

		// 後隙時間終了チェック
		if (_fTimer >= ATTACK_RECOVERY_TIME)
		{
			return std::make_shared<Approach>();
		}

		return nullptr;
	}

	// ターゲットを見失ったとき
	void LostTarget::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		_fWaitTimer = 0.0f;
		_ePhase = Phase::LOOK_AROUND;// 見渡しから
		_lookCnt = 0;

		// 現在の向きを基準の角度とする
		_fBaseAngle = atan2f(owner->GetDir().x, owner->GetDir().z);
		_vBaseDir = owner->GetDir();
		_vBaseDir.y = 0.0f;
		_vBaseDir = VNorm(_vBaseDir);

		// 最初の見渡し方向を設定
		SetNextLookDir();

		StopMove(owner);

		// アニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("enemy_idle_01", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
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

			// 現在の向きと目標方向の角度差を計算
			VECTOR vCurrentDir = owner->GetDir();
			float dot = VDot(vCurrentDir, _vLookDir);
			if (dot > 1.0f) { dot = 1.0f; }
			if (dot < -1.0f) { dot = -1.0f; }
			float diffDeg = acosf(dot) * RADIAN_TO_DEGREE;

			// 目標方向に到達したら次の見渡し方向へ
			if (diffDeg <= LOST_LOOK_ARRIVE_THRESHOLD)
			{
				_lookCnt++;

				if (_lookCnt >= LOST_LOOK_COUNT)
				{
					_ePhase = Phase::TO_BASE;
				}
				else
				{
					// 次の見渡し方向設定
					SetNextLookDir();
				}
			}
		}
		else if (_ePhase == Phase::TO_BASE)
		{
			// 基準方向へ回転
			RotateToTarget(owner, _vBaseDir, LOST_ROTATE_SPEED);

			// 基準方向への到達判定
			VECTOR vCurrentDir = owner->GetDir();
			float dot = VDot(vCurrentDir, _vBaseDir);
			if (dot > 1.0f) { dot = 1.0f; }
			if (dot < -1.0f) { dot = -1.0f; }
			float diffDeg = acosf(dot) * RADIAN_TO_DEGREE;

			if (diffDeg <= LOST_LOOK_ARRIVE_THRESHOLD)
			{
				_ePhase = Phase::WAIT;
				_fWaitTimer = 0.0f;
			}
		}
		else if (_ePhase == Phase::WAIT)
		{
			_fWaitTimer++;

			if (_fWaitTimer >= LOST_WAIT_TIME)
			{
				_ePhase = Phase::RETURN_HOME;

				// アニメーション設定
				AnimManager* animManager = owner->GetAnimManager();
				if (animManager)
				{
					animManager->ChangeAnimationByName("enemy_walk_01", BLEND_FRAME, ANIM_LOOP_COUNT);
				}
			}
		}
		else// RETURN_HOME
		{
			// 初期位置への距離計算
			VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
			float dist = VSize(vToHome);

			if (dist <= LOST_NEARBY_HOME)
			{
				return std::make_shared<Idle>();
			}

			// 初期位置方向へ移動
			VECTOR vDir = mymath::FlattenVector(vToHome);
			RotateToTarget(owner, vDir, SMOOTH_ROTATE_SPEED);
			MoveToTarget(owner, vDir, owner->GetEnemyParam().fMoveSpeed);
		}

		return nullptr;
	}

	void LostTarget::UpdateSearch(Enemy* owner)
	{
		owner->SetTargetDetected(Normal::IsTargetVisibleFan(owner));
	}

	void LostTarget::SetNextLookDir()
	{
		// lookCntの偶数奇数で左右交互に振り分け
		float sign = (_lookCnt % 2 == 0) ? 1.0f : -1.0f;
		float fTargetAngle = _fBaseAngle + sign * LOST_LOOK_ANGLE * DEGREE_TO_RADIAN;
		_vLookDir = VGet(sinf(fTargetAngle), 0.0f, cosf(fTargetAngle));
		_vLookDir = VNorm(_vLookDir);
	}

}
