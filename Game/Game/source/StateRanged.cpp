#include "StateRanged.h"
#include "Enemy.h"

namespace 
{
	// 発射位置オフセット
	constexpr auto SHOOT_OFFSET_Z = 20.0f;// 発射位置前方オフセット
	constexpr auto SHOOT_OFFSET_Y = 40.0f;// 発射位置高さオフセット

	// 発射制御
	constexpr auto TURN_LOCK_TIME = 10.0f;// 発射の何フレーム前になったら向きを固定するか

	// 弾パラメータ
	constexpr auto BULLET_RADIUS = 10.0f;// 弾の当たり判定半径
	constexpr auto BULLET_SPEED = 15.0f;// 弾の速度
	constexpr auto BULLET_LIFETIME = 120.0f;// 弾の寿命(フレーム数)

	// 見渡し動作
	constexpr auto LOOK_AROUND_INTERVAL = 120.0f;// 見渡す間隔(フレーム数)
	constexpr auto LOOK_AROUND_ANGLE = 45.0f;// 左右を向く角度(度)
	constexpr auto LOOK_AROUND_SPEED = 0.1f;// 見渡しの回転速度(度/フレーム)

	// 攻撃時の判定係数
	constexpr auto ATTACK_AIM_DOT_THRESHOLD = 0.9f;// 発射可能な内積閾値(ほぼ正面)
	constexpr auto ATTACK_MOVE_BACK_DOT_THRESHOLD = 0.7f;// 後退する内積閾値(45度以内)
	constexpr auto ATTACK_VISION_RANGE_RATIO = 1.2f;// 視界喪失距離の係数

	// 角度補正
	constexpr auto ANGLE_EPSILON = 0.01f;// 角度到達判定の閾値
}

namespace Ranged
{
	bool IsTargetVisible(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) { return false; }

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);

		// 距離チェック
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }

		// 角度チェック
		VECTOR vToTargetNorm = VNorm(vToTarget);
		float dot = VDot(owner->GetDir(), vToTargetNorm);
		if (dot < owner->GetEnemyParam().fVisionCos) { return false; }

		// 障害物チェック

		return true;// 視界内
	}





	// 待機
	void Idle::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fLookAroundTimer = 0.0f;

		// 見渡し状態初期化
		_bLookingLeft = true;// 最初は左を向く
		_bisRotating = false;
		_vInitialDir = owner->GetDir();

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) 
	{
		// ターゲット発見チェック
		if (owner->IsTargetDetected()) 
		{
			return std::make_shared<Notice>();// 発見状態へ
		}

		// タイマー更新
		_fTimer++;
		_fLookAroundTimer++;

		// 見渡し処理
		{
			// 次の目標設定タイミングチェック
			if (_fLookAroundTimer >= LOOK_AROUND_INTERVAL && !_bisRotating)
			{
				// 目標角度計算
				float baseAngle = atan2f(_vInitialDir.x, _vInitialDir.z);
				float lookAngleRad = LOOK_AROUND_ANGLE * DEGREE_TO_RADIAN;

				if (_bLookingLeft)
				{
					_fTargetAngle = baseAngle + lookAngleRad;// 左を向く
				}
				else
				{
					_fTargetAngle = baseAngle - lookAngleRad;// 右を向く
				}

				// 回転開始設定
				_bisRotating = true;
				_bLookingLeft = !_bLookingLeft;
				_fLookAroundTimer = 0.0f;
			}

			// 滑らか回転処理
			if (_bisRotating)
			{
				// 現在角度計算
				VECTOR vCurrentDir = owner->GetDir();
				float currentAngle = atan2f(vCurrentDir.x, vCurrentDir.z);

				// 角度差計算
				float diffAngle = _fTargetAngle - currentAngle;
				while (diffAngle <= -DX_PI_F) { diffAngle += DX_TWO_PI_F; }
				while (diffAngle > DX_PI_F) { diffAngle -= DX_TWO_PI_F; }

				// 旋回速度制限計算
				float turnSpeedRad = LOOK_AROUND_SPEED * DEGREE_TO_RADIAN;

				// 到達判定
				if (fabs(diffAngle) < ANGLE_EPSILON)
				{
					// 回転終了処理
					_bisRotating = false;
					VECTOR vNewDir = VGet(sinf(_fTargetAngle), 0.0f, cosf(_fTargetAngle));
					owner->SetDir(vNewDir);
				}
				else
				{
					// 回転継続処理
					if (diffAngle > turnSpeedRad) { diffAngle = turnSpeedRad; }
					else if (diffAngle < -turnSpeedRad) { diffAngle = -turnSpeedRad; }

					float newAngle = currentAngle + diffAngle;
					VECTOR vNewDir = VGet(sinf(newAngle), 0.0f, cosf(newAngle));
					owner->SetDir(vNewDir);
				}
			}
		}

		return nullptr;
	}

	void Idle::UpdateSearch(Enemy* owner)
	{
		// ターゲット視界チェック
		if (IsTargetVisible(owner))
		{
			owner->SetTargetDetected(true);
		}
		else
		{
			owner->SetTargetDetected(false);
		}
	}





	// 発見
	void Notice::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// ここでアニメーション設定

		// ターゲット方向設定
		auto target = owner->GetTarget();
		if (target) 
		{
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
			VECTOR vToTargetNorm = VNorm(vToTarget);
			owner->SetDir(vToTargetNorm);
		}
	}

	std::shared_ptr<EnemyState> Notice::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// 発見時間経過チェック
		if (_fTimer >= owner->GetEnemyParam().fDetectTime) 
		{
			return std::make_shared<Attack>();// 攻撃状態へ
		}

		return nullptr;
	}





	// 攻撃
	void Attack::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_shotTimer = 0.0f;

		// 移動停止
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Attack::Update(Enemy* owner) 
	{
		// ターゲット存在チェック
		auto target = owner->GetTarget();
		if (!target)
		{
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// 待機状態へ
		}

		// タイマー更新
		_fTimer++;
		_shotTimer++;

		// パラメータ取得
		const auto& param = owner->GetEnemyParam();
		float interval = param.fAttackInterval;
		float moveBackRange = param.fAttackRange;
		float moveSpeed = param.fMoveSpeed;

		// ターゲットへのベクトル計算
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		float dist = VSize(vToTarget);
		VECTOR vToTargetNorm = VNorm(vToTarget);
		VECTOR vDir = owner->GetDir();
		float dot = VDot(vDir, vToTargetNorm);

		// 視界喪失チェック
		{
			// 距離チェック
			bool bIsOutRange = (dist > param.fVisionRange * ATTACK_VISION_RANGE_RATIO);

			// 角度チェック
			bool bIsOutAngle = (dot < param.fVisionCos);

			// 視界喪失判定
			if (bIsOutRange || bIsOutAngle) 
			{
				owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
				owner->SetTargetDetected(false);
				return std::make_shared<Idle>();// 待機状態へ
			}
		}

		// 回転処理
		{
			// 発射直前以外は追尾
			if (_shotTimer < interval - TURN_LOCK_TIME) 
			{
				// 目標角度計算
				float targetAngle = atan2f(vToTarget.x, vToTarget.z);

				// 現在角度計算
				VECTOR vCurrentDir = owner->GetDir();
				float currentAngle = atan2f(vCurrentDir.x, vCurrentDir.z);

				// 角度差計算
				float diffAngle = targetAngle - currentAngle;
				while (diffAngle <= -DX_PI_F) { diffAngle += DX_TWO_PI_F; }
				while (diffAngle > DX_PI_F) { diffAngle -= DX_TWO_PI_F; }

				// 旋回速度制限適用
				float turnSpeedRad = param.fTurnSpeed * DEGREE_TO_RADIAN;
				if (diffAngle > turnSpeedRad) { diffAngle = turnSpeedRad; }
				else if (diffAngle < -turnSpeedRad) { diffAngle = -turnSpeedRad; }

				// 方向ベクトル設定
				float newAngle = currentAngle + diffAngle;
				VECTOR vNewDir = VGet(sinf(newAngle), 0.0f, cosf(newAngle));
				owner->SetDir(vNewDir);
			}
		}

		// 移動処理
		{
			VECTOR vMove = VGet(0.0f, 0.0f, 0.0f);

			// 後退判定
			if (dist < moveBackRange && dot > ATTACK_MOVE_BACK_DOT_THRESHOLD)
			{
				// XZ平面ベクトル計算
				VECTOR vToTargetXZ = vToTarget;
				vToTargetXZ.y = 0.0f;
				VECTOR vToTargetXZNorm = VNorm(vToTargetXZ);

				// 後退ベクトル計算
				VECTOR vBack = VScale(vToTargetXZNorm, -1.0f);
				vMove = VScale(vBack, moveSpeed);
			}

			owner->SetMove(vMove);
		}

		// 発射処理
		if (_shotTimer >= interval && dot > ATTACK_AIM_DOT_THRESHOLD) 
		{
			Shoot(owner);

			// エフェクト再生処理
			int playingHandle = EffectServer::GetInstance()->Play("Laser", owner->GetPos());
			if (playingHandle != -1)
			{
				// エフェクト角度計算
				VECTOR vDir = owner->GetDir();
				float yaw = atan2f(vDir.x, vDir.z) * RADIAN_TO_DEGREE;

				// エフェクト回転設定
				EffectServer::GetInstance()->SetRot(playingHandle, VGet(0.0f, yaw, 0.0f));
			}

			_shotTimer = 0.0f;
		}

		return nullptr;
	}

	void Attack::Shoot(Enemy* owner)
	{
		// 発射位置計算
		VECTOR vMuzzlePos = VAdd(
			owner->GetPos(),
			VScale(owner->GetDir(), SHOOT_OFFSET_Z)
		);
		vMuzzlePos.y += SHOOT_OFFSET_Y;

		// 弾生成
		owner->SpawnBullet(vMuzzlePos, owner->GetDir(), BULLET_RADIUS, BULLET_SPEED, BULLET_LIFETIME);
	}
}
