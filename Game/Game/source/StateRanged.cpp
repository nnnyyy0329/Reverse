#include "StateRanged.h"
#include "Enemy.h"

namespace 
{
	// 距離判定用定数
	constexpr auto NEARBY_HOME = 10.0f;					// 初期位置到達判定距離
	constexpr auto WANDER_HOME_RETURN_RATIO = 0.8f;		// 徘徊時の帰還判定比率
	constexpr auto SHOT_RANGE_MIN = 300.0f;				// 射撃最小距離
	constexpr auto SHOT_RANGE_MAX = 600.0f;				// 射撃最大距離
	constexpr auto RETREAT_TRIGGER_DISTANCE = 250.0f;	// 後退開始距離
	constexpr auto APPROACH_STOP_DISTANCE = 400.0f;		// 接近停止距離

	// 時間制御用定数
	constexpr auto SHOT_CHARGE_TIME = 60.0f;			// 射撃溜め時間
	constexpr auto SHOT_EXECUTE_TIME = 30.0f;			// 射撃実行時間
	constexpr auto SHOT_RECOVERY_TIME = 60.0f;			// 射撃後隙時間
	constexpr auto SHOT_INTERVAL_TIME = 90.0f;			// 射撃間隔時間
	constexpr auto BLEND_FRAME = 1.0f;					// アニメーションブレンドフレーム数

	// 速度制御用定数
	constexpr auto SMOOTH_ROTATE_SPEED = 5.0f;			// スムーズ回転速度
	constexpr auto APPROACH_SPEED = 1.0f;				// 接近速度(ゆっくり)
	constexpr auto RETREAT_SPEED = 1.5f;				// 後退速度

	// 確率制御用定数
	constexpr auto WANDER_ANGLE_RANDOM_RANGE = 90;		// 徘徊時の角度ランダム範囲(度)
	constexpr auto WANDER_ANGLE_RANDOM_OFFSET = 45;		// 徘徊時の角度ランダムオフセット(度)
	constexpr auto WANDER_FULL_ANGLE = 359;				// 徘徊時の全角度範囲(度)

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





	// 徘徊
	void Wander::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定

		// 移動方向決定
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





	// 攻撃
	void Approach::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Approach::Update(Enemy* owner)
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// 距離計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);
		const auto& param = owner->GetEnemyParam();

		// 追跡限界チェック
		if (dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// 射撃射程内チェック
		if (dist <= APPROACH_STOP_DISTANCE)
		{
			return std::make_shared<ShotStart>();
		}

		// ターゲット方向へ回転処理
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// 移動処理(ゆっくり接近)
		VECTOR vMove = VScale(vDir, APPROACH_SPEED);
		owner->SetMove(vMove);

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
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
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

		// 射撃射程内に入った場合
		if (dist >= SHOT_RANGE_MIN && dist <= SHOT_RANGE_MAX)
		{
			return std::make_shared<ShotCharge>();
		}

		// 射程外の場合、距離に応じて接近か後退か
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		if (dist > SHOT_RANGE_MAX)
		{
			// 接近
			VECTOR vMove = VScale(vDir, APPROACH_SPEED);
			owner->SetMove(vMove);
		}
		else if (dist < SHOT_RANGE_MIN)
		{
			// 後退
			VECTOR vMove = VScale(vDir, -RETREAT_SPEED);
			owner->SetMove(vMove);
		}
		else
		{
			// 射程内なら停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
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

		// 弾発射処理(一回のみ)
		if (!_bHasShot)
		{
			auto target = owner->GetTarget();
			if (target)
			{
				// 弾発射位置計算
				VECTOR vOwnerPos = owner->GetPos();
				VECTOR vDir = owner->GetDir();

				// 前方と高さの調整
				VECTOR vSpawnPos = VAdd(vOwnerPos, VGet(0.0f, BULLET_SPAWN_OFFSET_Y, 0.0f));
				vSpawnPos = VAdd(vSpawnPos, VScale(vDir, BULLET_SPAWN_OFFSET_Z));

				// ターゲットへの方向計算
				VECTOR vToTarget = VSub(target->GetPos(), vSpawnPos);
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
		}

		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (target)
		{
			// 距離計算
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			float dist = VSize(vToTarget);

			// ターゲット方向へ追従処理
			VECTOR vDir = VNorm(vToTarget);
			owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

			// ターゲットが近い場合は後退
			if (dist < RETREAT_TRIGGER_DISTANCE)
			{
				VECTOR vMove = VScale(vDir, -RETREAT_SPEED);
				owner->SetMove(vMove);
			}
			else
			{
				// それ以外はその場で
				owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
			}
		}
		else
		{
			// ターゲット不在時は停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
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

		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (target)
		{
			// 距離計算
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			float dist = VSize(vToTarget);

			// ターゲット方向へ追従処理
			VECTOR vDir = VNorm(vToTarget);
			owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

			// ターゲットが近い場合は後退
			if (dist < RETREAT_TRIGGER_DISTANCE)
			{
				VECTOR vMove = VScale(vDir, -RETREAT_SPEED);
				owner->SetMove(vMove);
			}
			else
			{
				// それ以外はその場で
				owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
			}
		}
		else
		{
			// ターゲット不在時は停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
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
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
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

		// 追跡距離限界チェック
		if (dist > param.fChaseLimitRange)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();
		}

		// ターゲット方向へ追従処理
		VECTOR vDir = VNorm(vToTarget);
		owner->SmoothRotateTo(vDir, SMOOTH_ROTATE_SPEED);

		// ターゲットが近づいてきた場合は後退
		if(dist < RETREAT_TRIGGER_DISTANCE)
		{
			VECTOR vMove = VScale(vDir, -RETREAT_SPEED);
			owner->SetMove(vMove);
		}
		else
		{
			// それ以外はその場で
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
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
		owner->SetTargetDetected(Ranged::IsTargetVisible(owner));
	}
}
