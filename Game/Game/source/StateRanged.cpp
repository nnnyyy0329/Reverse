#include "StateRanged.h"
#include "Enemy.h"
#include "EnemyState_impl.h"

namespace 
{
	// 距離判定用定数
	constexpr auto SHOT_RANGE_MIN = 300.0f;				// 射撃最小距離
	constexpr auto SHOT_RANGE_MAX = 400.0f;				// 射撃最大距離
	constexpr auto RETREAT_TRIGGER_DISTANCE = 150.0f;	// 後退開始距離
	constexpr auto APPROACH_STOP_DISTANCE = 400.0f;		// 接近停止距離
	constexpr auto LOST_NEARBY_HOME = 10.0f;			// 帰還完了判定距離

	// 時間制御用定数
	constexpr auto WANDER_LOOK_MIN_TIME = 60.0f;		// 徘徊時の最小視線停止時間
	constexpr auto WANDER_LOOK_RANDOM_TIME = 60.0f;		// 徘徊時の視線停止ランダム追加時間
	constexpr auto SHOT_CHARGE_TIME = 120.0f;			// 射撃溜め時間
	constexpr auto SHOT_EXECUTE_TIME = 130.0f;			// 射撃実行時間
	constexpr auto SHOT_RECOVERY_TIME = 60.0f;			// 射撃後隙時間
	constexpr auto SHOT_INTERVAL_TIME = 90.0f;			// 射撃間隔時間
	constexpr float SHOT_BULLET_TIME = 90.0f;			// 実際に弾が発射される時間(アニメーション準拠)
	constexpr auto LOST_WAIT_TIME = 60.0f;				// 帰還前の待機時間
	constexpr auto RETREAT_TIME = 120.0f;				// 後退時間

	// 速度制御用定数
	constexpr auto WANDER_ROTATE_SPEED = 2.0f;			// 徘徊時の回転速度
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度
	constexpr auto APPROACH_SPEED = 1.0f;				// 接近速度(ゆっくり)
	constexpr auto RETREAT_SPEED = 2.0f;				// 後退速度
	constexpr auto LOST_ROTATE_SPEED = 1.5f;			// 見渡し回転速度

	// 確率制御用定数
	constexpr auto WANDER_ANGLE_RANDOM_RANGE = 180;		// 徘徊時の角度ランダム範囲(度)
	constexpr auto WANDER_ANGLE_RANDOM_OFFSET = 90;		// 徘徊時の角度ランダムオフセット(度)

	// アニメーション制御用定数
	constexpr auto BLEND_FRAME = 10.0f;					// アニメーションブレンドフレーム数
	constexpr auto ANIM_LOOP_COUNT = 0;					// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;					// アニメーション再生回数
	constexpr auto ANIM_SPEED_HALF = 0.5f;				// アニメーション再生速度(半分)

	// 弾設定用定数
	constexpr auto BULLET_SPEED = 15.0f;				// 弾速度
	constexpr auto BULLET_RADIUS = 10.0f;				// 弾半径
	constexpr auto BULLET_LIFETIME = 180;				// 弾生存時間(フレーム)
	constexpr auto BULLET_SPAWN_OFFSET_Y = 100.0f;		// 弾発射Y座標オフセット
	constexpr auto BULLET_SPAWN_OFFSET_Z = 50.0f;		// 弾発射前方オフセット
	constexpr auto BULLET_DAMAGE = 10.0f;				// 弾ダメージ

	// 時間ランダム幅定数
	constexpr auto IDLE_TIME_RANGE = 30.0f;				// 待機時間のランダム幅
	constexpr auto NOTICE_TIME_RANGE = 10.0f;			// 発見硬直のランダム幅
	constexpr auto SHOT_INTERVAL_TIME_RANGE = 20.0f;	// 射撃間隔時間のランダム幅

	// 見渡し関連
	constexpr auto LOST_LOOK_ARRIVE_THRESHOLD = 2.0f;	// 見渡し到達判定角度
	constexpr auto LOST_LOOK_ANGLE = 45.0f;				// 見渡し回転角度
	constexpr auto LOST_LOOK_COUNT = 3;					// 見渡し回数

	// 視界チェック用定数
	constexpr auto FAN_VISON_HALF_ANGLE = 45.0f;// 扇形視界の半角(度)
	const float FAN_VISION_COS = cosf(FAN_VISON_HALF_ANGLE * DEGREE_TO_RADIAN);// cos値
	constexpr auto TARGET_DETECT_RADIUS = 30.0f;// ターゲット検出に幅を持たせる(半径)

	// エフェクト関連
	constexpr auto ATTACK_EFFECT_OFFSET_Y = 100.0f;		// 攻撃エフェクトYオフセット
	constexpr auto ATTACK_EFFECT_OFFSET_Z = 100.0f;		// 攻撃エフェクト前方オフセット
}

namespace Ranged
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

		// 時間にばらつきを持たせる
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fIdleTime, IDLE_TIME_RANGE);

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
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
		owner->SetTargetDetected(Ranged::IsTargetVisibleFan(owner));
	}





	// 徘徊(その場で周囲を見渡す)
	void Wander::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		// 視線停止時間を決定(ばらつきを持たせる)
		_fLookDuration = CalcRandomRangeTime(WANDER_LOOK_MIN_TIME, WANDER_LOOK_RANDOM_TIME);

		// ランダムな角度を決定
		float fRandomAngle = CalcRandomRangeTime(-WANDER_ANGLE_RANDOM_OFFSET, WANDER_ANGLE_RANDOM_RANGE);
		float fCurrentAngle = atan2f(owner->GetDir().x, owner->GetDir().z);
		float fTargetAngle = fCurrentAngle + fRandomAngle;

		// 目標方向ベクトルを計算
		_vTargetDir = VGet(sinf(fTargetAngle), 0.0f, cosf(fTargetAngle));
		_vTargetDir = VNorm(_vTargetDir);

		// ここでアニメーション設定
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

		// その場にとどまる
		StopMove(owner);

		// 目標方向へゆっくり回転
		RotateToTarget(owner, _vTargetDir, WANDER_ROTATE_SPEED);

		// 時間経過チェック
		if (_fTimer >= _fLookDuration)
		{
			return std::make_shared<Idle>();
		}

		return nullptr;
	}

	void Wander::UpdateSearch(Enemy* owner)
	{
		owner->SetTargetDetected(Ranged::IsTargetVisibleFan(owner));
	}





	// 発見
	void Notice::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(owner->GetEnemyParam().fDetectTime, NOTICE_TIME_RANGE);

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_look_00", BLEND_FRAME, ANIM_PLAY_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> Notice::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ターゲット方向へ回転処理
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

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_back_00", BLEND_FRAME, ANIM_LOOP_COUNT);
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
		if(result) { return result; }

		// 移動可能範囲外チェック
		auto areaResult = TransitionToLostOutsideArea<LostTarget>(owner);
		if (areaResult) { return areaResult; }

		// 射撃射程内チェック
		if (targetInfo.fDist <= APPROACH_STOP_DISTANCE)
		{
			return std::make_shared<ShotStart>();
		}

		// ターゲット方向へ回転
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

		// ゆっくり接近
		MoveToTarget(owner, targetInfo.vDir, APPROACH_SPEED);

		return nullptr;
	}


	// 後退
	void Retreat::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_back_00", BLEND_FRAME, ANIM_LOOP_COUNT);
		}
	}

	std::shared_ptr<EnemyState> Retreat::Update(Enemy* owner)
	{
		_fTimer++;

		auto targetInfo = GetTargetInfo(owner);

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

		// ターゲット方向へ回転しつつ後退
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);

		if(_fTimer >= RETREAT_TIME)
		{
			return std::make_shared<ShotStart>();
		}

		return nullptr;
	}



	// 射撃ステート開始
	void ShotStart::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> ShotStart::Update(Enemy* owner)
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

		// 射撃射程内に入った場合
		if (targetInfo.fDist >= SHOT_RANGE_MIN && targetInfo.fDist <= SHOT_RANGE_MAX)
		{
			return std::make_shared<ShotCharge>();
		}

		// ターゲット方向へ回転
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

		// 近すぎる場合は後退
		if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
		{
			return std::make_shared<Retreat>();
		}

		// 遠すぎる場合は接近
		if (targetInfo.fDist > SHOT_RANGE_MAX)
		{
			return std::make_shared<Approach>();
		}

		StopMove(owner);

		return nullptr;
	}

	// 射撃溜め
	void ShotCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_attack_00", BLEND_FRAME, ANIM_PLAY_COUNT);
		}
	}

	std::shared_ptr<EnemyState> ShotCharge::Update(Enemy* owner)
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
		if (_fTimer >= SHOT_CHARGE_TIME)
		{
			return std::make_shared<ShotExecute>();
		}

		return nullptr;
	}

	// 射撃実行
	void ShotExecute::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_bHasShot = false;
		_fShotTimer = 0.0f;

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_attack_01", BLEND_FRAME, ANIM_PLAY_COUNT);
		}
	}

	std::shared_ptr<EnemyState> ShotExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;
		_fShotTimer++;

		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// 弾発射処理(一回のみ)
		if (_fShotTimer >= SHOT_BULLET_TIME)
		{
			if (!_bHasShot && targetInfo.bExist)
			{
				owner->SpawnBullet(GetBulletConfig(owner));

				// エフェクト
				{
					VECTOR dir = owner->GetDir();
					float rotY = atan2f(dir.x, dir.z);

					// 前方方向へのオフセットを加算
					VECTOR efPos = VAdd(owner->GetPos(), VGet(0.0f, ATTACK_EFFECT_OFFSET_Y, 0.0f));
					efPos = VAdd(efPos, VScale(dir, ATTACK_EFFECT_OFFSET_Z));
					EffectServer::GetInstance()->Play("Normal_Attack02", efPos);
				}

				// SE
				SoundServer::GetInstance()->Play("SE_Ranged_Shot", DX_PLAYTYPE_BACK);

				_bHasShot = true;
			}
		}

		// ターゲットが存在する場合
		if (targetInfo.bExist)
		{
			// ターゲット方向へ追従
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);
		}

		StopMove(owner);

		// 射撃実行時間終了チェック
		if (_fTimer >= SHOT_EXECUTE_TIME)
		{
			return std::make_shared<ShotRecovery>();
		}

		return nullptr;
	}

	// 弾の情報設定
	BulletConfig ShotExecute::GetBulletConfig(Enemy* owner)
	{
		auto targetInfo = GetTargetInfo(owner);

		// 弾発射位置計算
		VECTOR vOwnerPos = owner->GetPos();
		VECTOR vDir = owner->GetDir();

		VECTOR vSpawnPos = VAdd(vOwnerPos, VGet(0.0f, BULLET_SPAWN_OFFSET_Y, 0.0f));
		vSpawnPos = VAdd(vSpawnPos, VScale(vDir, BULLET_SPAWN_OFFSET_Z));

		// ターゲットへの方向計算
		VECTOR vToTarget = VSub(targetInfo.target->GetPos(), vSpawnPos);
		VECTOR vBulletDir = VNorm(vToTarget);

		// 弾の情報
		BulletConfig config;

		// 情報設定
		config.bulletType = BULLET_TYPE::NORMAL;
		config.shooterType = CHARA_TYPE::ENEMY;
		config.startPos = vSpawnPos;
		config.dir = vBulletDir;
		config.radius = BULLET_RADIUS;
		config.damage = BULLET_DAMAGE;
		config.speed = BULLET_SPEED;
		config.lifeTime = BULLET_LIFETIME;

		return config;
	}

	// 射撃後隙
	void ShotRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
		AnimManager* animManager = owner->GetAnimManager();
		if (animManager)
		{
			animManager->ChangeAnimationByName("Senemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
		}
	}

	std::shared_ptr<EnemyState> ShotRecovery::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		if (targetInfo.bExist)
		{
			// ターゲット方向へ追従
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

			// ターゲットが近い場合は後退
			/*if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
			{
				MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);
			}
			else
			{
				StopMove(owner);
			}*/
			StopMove(owner);
		}
		else
		{
			StopMove(owner);
		}

		// 後隙時間終了チェック
		if (_fTimer >= SHOT_RECOVERY_TIME)
		{
			return std::make_shared<ShotInterval>();
		}

		return nullptr;
	}

	// 射撃間隔
	void ShotInterval::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間ランダム設定
		_fTargetTimer = CalcRandomRangeTime(SHOT_INTERVAL_TIME, SHOT_INTERVAL_TIME_RANGE);

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> ShotInterval::Update(Enemy* owner)
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

		// ターゲット方向へ追従
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

		// 近すぎる場合は後退
		if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
		{
			return std::make_shared<Retreat>();
		}

		StopMove(owner);

		// 射撃間隔時間終了チェック
		if (_fTimer >= _fTargetTimer)
		{
			return std::make_shared<ShotStart>();
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
			animManager->ChangeAnimationByName("Senemy_miss_00", BLEND_FRAME, ANIM_PLAY_COUNT);
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
					animManager->ChangeAnimationByName("Senemy_back_00", BLEND_FRAME, ANIM_LOOP_COUNT);
				}
			}
		}
		else if (_ePhase == Phase::RETURN_HOME)
		{
			// 初期位置への距離計算
			VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
			float dist = VSize(vToHome);

			if (dist <= LOST_NEARBY_HOME)
			{
				_ePhase = Phase::SET_DIR;

				// アニメーション設定
				AnimManager* animManager = owner->GetAnimManager();
				if (animManager)
				{
					animManager->ChangeAnimationByName("Senemy_idle_00", BLEND_FRAME, ANIM_LOOP_COUNT, ANIM_SPEED_HALF);
				}
			}

			// 初期位置方向へ移動
			VECTOR vDir = mymath::FlattenVector(vToHome);
			RotateToTarget(owner, vDir, SMOOTH_ROTATE_SPEED);
			MoveToTarget(owner, vDir, owner->GetEnemyParam().fMoveSpeed);
		}
		else// SET_DIR
		{
			StopMove(owner);

			VECTOR vHomeDir = owner->GetHomeDir();
			RotateToTarget(owner, vHomeDir, SMOOTH_ROTATE_SPEED);

			// 初期向きへの到達判定
			VECTOR vCurrentDir = owner->GetDir();
			float dot = VDot(vCurrentDir, vHomeDir);
			if (dot > 1.0f) { dot = 1.0f; }
			if (dot < -1.0f) { dot = -1.0f; }
			float diffDeg = acosf(dot) * RADIAN_TO_DEGREE;

			if (diffDeg <= LOST_LOOK_ARRIVE_THRESHOLD)
			{
				return std::make_shared<Idle>();
			}
		}

		return nullptr;
	}

	void LostTarget::UpdateSearch(Enemy* owner)
	{
		owner->SetTargetDetected(Ranged::IsTargetVisibleFan(owner));
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
