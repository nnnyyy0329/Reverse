#include "StateRanged.h"
#include "Enemy.h"
#include "EnemyState_impl.h"

namespace 
{
	// 距離判定用定数
	constexpr auto NEARBY_HOME = 10.0f;					// 初期位置到達判定距離
	constexpr auto SHOT_RANGE_MIN = 300.0f;				// 射撃最小距離
	constexpr auto SHOT_RANGE_MAX = 600.0f;				// 射撃最大距離
	constexpr auto RETREAT_TRIGGER_DISTANCE = 250.0f;	// 後退開始距離
	constexpr auto APPROACH_STOP_DISTANCE = 400.0f;		// 接近停止距離

	// 時間制御用定数
	constexpr auto WANDER_LOOK_MIN_TIME = 60.0f;		// 徘徊時の最小視線停止時間
	constexpr auto WANDER_LOOK_RANDOM_TIME = 60.0f;		// 徘徊時の視線停止ランダム追加時間
	constexpr auto SHOT_CHARGE_TIME = 60.0f;			// 射撃溜め時間
	constexpr auto SHOT_EXECUTE_TIME = 30.0f;			// 射撃実行時間
	constexpr auto SHOT_RECOVERY_TIME = 60.0f;			// 射撃後隙時間
	constexpr auto SHOT_INTERVAL_TIME = 90.0f;			// 射撃間隔時間
	constexpr auto BLEND_FRAME = 1.0f;					// アニメーションブレンドフレーム数

	// 速度制御用定数
	constexpr auto WANDER_ROTATE_SPEED = 2.0f;			// 徘徊時の回転速度
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度
	constexpr auto APPROACH_SPEED = 1.0f;				// 接近速度(ゆっくり)
	constexpr auto RETREAT_SPEED = 1.5f;				// 後退速度

	// 確率制御用定数
	constexpr auto WANDER_ANGLE_RANDOM_RANGE = 180;		// 徘徊時の角度ランダム範囲(度)
	constexpr auto WANDER_ANGLE_RANDOM_OFFSET = 90;		// 徘徊時の角度ランダムオフセット(度)

	// アニメーション制御用定数
	constexpr auto ANIM_LOOP_COUNT = 0;					// アニメーションループ回数(0=無限)
	constexpr auto ANIM_PLAY_COUNT = 1;					// アニメーション再生回数
	constexpr auto ANIM_SPEED_NORMAL = 1.0f;			// アニメーション再生速度(通常)

	// 弾設定用定数
	constexpr auto BULLET_SPEED = 15.0f;				// 弾速度
	constexpr auto BULLET_RADIUS = 10.0f;				// 弾半径
	constexpr auto BULLET_LIFETIME = 180;				// 弾生存時間(フレーム)
	constexpr auto BULLET_SPAWN_OFFSET_Y = 100.0f;		// 弾発射Y座標オフセット
	constexpr auto BULLET_SPAWN_OFFSET_Z = 50.0f;		// 弾発射前方オフセット
}

namespace Ranged
{
	bool IsTargetVisible(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// ベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());

		// 距離チェック
		float dist = VSize(vToTarget);
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 障害物チェック

		return true;// 視界内
	}





	// 待機
	void Idle::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 時間にばらつきを持たせる
		_fTargetTimer = CalcOffsetTime(owner, owner->GetEnemyParam().fIdleTime);

		// ここでアニメーション設定
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
		// 視界判定結果を設定
		owner->SetTargetDetected(Ranged::IsTargetVisible(owner));
	}





	// 徘徊(その場で周囲を見渡す)
	void Wander::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		// 視線停止時間を決定(ばらつきを持たせる)
		_fLookDuration = WANDER_LOOK_MIN_TIME + static_cast<float>(GetRand(static_cast<int>(WANDER_LOOK_RANDOM_TIME)));

		// ランダムな角度を決定
		float fRandomAngle = static_cast<float>(GetRand(WANDER_ANGLE_RANDOM_RANGE) - WANDER_ANGLE_RANDOM_OFFSET) * DEGREE_TO_RADIAN;
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
		// 視界判定結果を設定
		owner->SetTargetDetected(Ranged::IsTargetVisible(owner));
	}





	// 発見
	void Notice::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
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

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Approach::Update(Enemy* owner)
	{
		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return HandleNoTarget<Idle>(owner);
		}

		// 追跡限界距離チェック
		auto result = CheckChaseLimitAndHandle<Idle>(owner, targetInfo.fDist);
		if(result) { return result; }

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
			return HandleNoTarget<Idle>(owner);
		}

		// タイマー更新
		_fTimer++;

		// 追跡限界距離チェック
		auto result = CheckChaseLimitAndHandle<Idle>(owner, targetInfo.fDist);
		if (result) { return result; }

		// 射撃射程内に入った場合
		if (targetInfo.fDist >= SHOT_RANGE_MIN && targetInfo.fDist <= SHOT_RANGE_MAX)
		{
			return std::make_shared<ShotCharge>();
		}

		// ターゲット方向へ回転
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

		// 射程外の場合、距離に応じて接近か後退
		if (targetInfo.fDist > SHOT_RANGE_MAX)
		{
			MoveToTarget(owner, targetInfo.vDir, APPROACH_SPEED);
		}
		else if (targetInfo.fDist < SHOT_RANGE_MIN)
		{
			MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);
		}
		else
		{
			StopMove(owner);
		}

		return nullptr;
	}

	// 射撃溜め
	void ShotCharge::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;
		// ここでアニメーション設定
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

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> ShotExecute::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// 弾発射処理(一回のみ)
		if (!_bHasShot && targetInfo.bExist)
		{
			// 弾発射位置計算
			VECTOR vOwnerPos = owner->GetPos();
			VECTOR vDir = owner->GetDir();

			VECTOR vSpawnPos = VAdd(vOwnerPos, VGet(0.0f, BULLET_SPAWN_OFFSET_Y, 0.0f));
			vSpawnPos = VAdd(vSpawnPos, VScale(vDir, BULLET_SPAWN_OFFSET_Z));

			// ターゲットへの方向計算
			VECTOR vToTarget = VSub(targetInfo.target->GetPos(), vSpawnPos);
			VECTOR vBulletDir = VNorm(vToTarget);

			// 弾発射
			owner->SpawnBullet(
				vSpawnPos,
				vBulletDir,
				BULLET_RADIUS,
				BULLET_SPEED,
				BULLET_LIFETIME
			);

			_bHasShot = true;
		}

		// ターゲットが存在する場合
		if (targetInfo.bExist)
		{
			// ターゲット方向へ追従
			RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

			// ターゲットが近い場合は後退
			if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
			{
				MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);
			}
			else
			{
				StopMove(owner);
			}
		}
		else
		{
			StopMove(owner);
		}

		// 射撃実行時間終了チェック
		if (_fTimer >= SHOT_EXECUTE_TIME)
		{
			return std::make_shared<ShotRecovery>();
		}

		return nullptr;
	}

	// 射撃後隙
	void ShotRecovery::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
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
			if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
			{
				MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);
			}
			else
			{
				StopMove(owner);
			}
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

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> ShotInterval::Update(Enemy* owner)
	{
		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		// ターゲット存在チェック
		if (!targetInfo.bExist)
		{
			return HandleNoTarget<Idle>(owner);
		}

		// タイマー更新
		_fTimer++;
		
		// 追跡限界距離チェック
		auto result = CheckChaseLimitAndHandle<Idle>(owner, targetInfo.fDist);
		if (result) { return result; }

		// ターゲット方向へ追従
		RotateToTarget(owner, targetInfo.vDir, SMOOTH_ROTATE_SPEED);

		// ターゲットが近づいてきた場合は後退
		if (targetInfo.fDist < RETREAT_TRIGGER_DISTANCE)
		{
			MoveToTarget(owner, targetInfo.vDir, -RETREAT_SPEED);
		}
		else
		{
			StopMove(owner);
		}

		// 射撃間隔時間終了チェック
		if (_fTimer >= SHOT_INTERVAL_TIME)
		{
			return std::make_shared<ShotStart>();
		}

		return nullptr;
	}





	// 帰還
	void ReturnHome::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> ReturnHome::Update(Enemy* owner)
	{
		// 索敵結果チェック
		if(owner->IsTargetDetected() )
		{
			return std::make_shared<Notice>();
		}

		// 初期位置への距離計算
		VECTOR vToHome = VSub(owner->GetHomePos(), owner->GetPos());
		float dist = VSize(vToHome);

		// 到達判定チェック
		if(dist <= NEARBY_HOME) 
		{
			return std::make_shared<Idle>();
		}

		// 初期位置方向へ回転・移動
		VECTOR vDir = VNorm(vToHome);
		RotateToTarget(owner, vDir, SMOOTH_ROTATE_SPEED);
		MoveToTarget(owner, vDir, owner->GetEnemyParam().fMoveSpeed);

		return nullptr;
	}

	void ReturnHome::UpdateSearch(Enemy* owner)
	{
		// 視界判定結果を設定
		owner->SetTargetDetected(Ranged::IsTargetVisible(owner));
	}
}
