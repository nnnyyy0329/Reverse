#include "StateRanged.h"
#include "Enemy.h"

namespace {
	constexpr auto SHOOT_OFFSET_Z = 20.0f;// 発射位置前方オフセット
	constexpr auto SHOOT_OFFSET_Y = 40.0f;// 発射位置高さオフセット

	constexpr auto TURN_LOCK_TIME = 10.0f;// 発射の何フレーム前になったら向きを固定するか

	constexpr auto BULLET_RADIUS = 10.0f;// 弾の当たり判定半径
	constexpr auto BULLET_SPEED = 15.0f;// 弾の速度
	constexpr auto BULLET_LIFETIME = 120.0f;// 弾の寿命(フレーム数)

	constexpr auto LOOK_AROUND_INTERVAL = 120.0f;// 見渡す間隔(フレーム数)
	constexpr auto LOOK_AROUND_ANGLE = 45.0f;// 左右を向く角度(度)
	constexpr auto LOOK_AROUND_SPEED = 0.1f;// 見渡しの回転速度(度/フレーム)
}

namespace Ranged
{
	bool IsTargetVisible(Enemy* owner)
	{
		auto target = owner->GetTarget();
		if (!target) return false;

		// 距離チェック
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());// ターゲットへのベクトル
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		if (dist > owner->GetEnemyParam().fVisionRange) { return false; }// 索敵距離外

		// 角度チェック
		VECTOR vDirToTarget = VNorm(vToTarget);// ターゲット方向(正規化)
		auto dot = VDot(owner->GetDir(), vDirToTarget);// 内積
		// dotがlimitCos未満なら視界外
		if (dot < owner->GetEnemyParam().fVisionCos) { return false; }

		// 障害物チェック

		return true;// 視界内
	}





	// 待機
	void Idle::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		_fLookAroundTimer = 0.0f;
		_bLookingLeft = true;// 最初は左を向く
		_bisRotating = false;// 回転中フラグを初期化
		_vInitialDir = owner->GetDir();// 初期方向を保存
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) {
		// 索敵結果を使用
		if (owner->IsTargetDetected()) {
			return std::make_shared<Detect>();// 発見状態へ
		}

		_fTimer++;
		_fLookAroundTimer++;

		// 見渡す:一定間隔で左右を向く
		// 旋回速度制限を適用して滑らかに回転させる
		{
			// 次の目標を設定するタイミング
			if (_fLookAroundTimer >= LOOK_AROUND_INTERVAL && !_bisRotating)
			{
				// 初期方向を基準に次の目標角度を計算
				auto baseAngle = atan2f(_vInitialDir.x, _vInitialDir.z);// 初期方向の角度
				auto lookAngleRad = LOOK_AROUND_ANGLE * DEGREE_TO_RADIAN;

				if (_bLookingLeft)
				{
					// 左を向く(+方向)
					_fTargetAngle = baseAngle + lookAngleRad;
				}
				else
				{
					// 右を向く(-方向)
					_fTargetAngle = baseAngle - lookAngleRad;
				}

				// 回転開始フラグを立てる
				_bisRotating = true;

				// 次は反対方向を向く
				_bLookingLeft = !_bLookingLeft;

				_fLookAroundTimer = 0.0f;// タイマーリセット
			}

			// 滑らかな回転処理
			if (_bisRotating)
			{
				// 現在の角度を計算
				VECTOR vCurrentDir = owner->GetDir();
				auto currentAngle = atan2f(vCurrentDir.x, vCurrentDir.z);

				// 角度差を計算(-PI ~ +PI の範囲に収める)
				auto diffAngle = _fTargetAngle - currentAngle;
				// 180度を超えないように補正
				while (diffAngle <= -DX_PI_F) diffAngle += DX_TWO_PI_F;
				while (diffAngle > DX_PI_F) diffAngle -= DX_TWO_PI_F;

				// 旋回速度の制限
				auto turnSpeedRad = LOOK_AROUND_SPEED * DEGREE_TO_RADIAN;

				// 目標角度に到達したかチェック
				if (fabs(diffAngle) < 0.01f)
				{
					// 目標角度に到達したので回転終了
					_bisRotating = false;
					// 目標角度に正確に合わせる
					VECTOR vNewDir = VGet(sinf(_fTargetAngle), 0.0f, cosf(_fTargetAngle));
					owner->SetDir(vNewDir);
				}
				else
				{
					// まだ目標角度に到達していないので回転を続ける
					// 差が速度制限を超えていたら、速度分だけ回す
					if (diffAngle > turnSpeedRad) { diffAngle = turnSpeedRad; }
					else if (diffAngle < -turnSpeedRad) { diffAngle = -turnSpeedRad; }

					// 新しい向きベクトルを計算して設定
					auto newAngle = currentAngle + diffAngle;
					VECTOR vNewDir = VGet(sinf(newAngle), 0.0f, cosf(newAngle));
					owner->SetDir(vNewDir);
				}
			}
		}

		return nullptr;
	}

	void Idle::UpdateSearch(Enemy* owner)
	{
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
	void Detect::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定

		// 発見時にプレイヤーの方向を向く
		auto target = owner->GetTarget();
		if (target) {
			VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());// ターゲットへのベクトル
			VECTOR vDir = VNorm(vToTarget);
			owner->SetDir(vDir);
		}
	}

	std::shared_ptr<EnemyState> Detect::Update(Enemy* owner) {
		_fTimer++;

		// 時間経過で
		if (_fTimer >= owner->GetEnemyParam().fDetectTime) {
			return std::make_shared<Attack>();// 攻撃状態へ
		}
	}





	// 攻撃
	void Attack::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		_shotTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Attack::Update(Enemy* owner) {
		auto target = owner->GetTarget();
		if (!target)
		{
			// 索敵結果をクリア
			owner->SetTargetDetected(false);
			return std::make_shared<Idle>();// ターゲットがいない場合は待機状態へ
		}

		_fTimer++;
		_shotTimer++;

		auto interval = owner->GetEnemyParam().fAttackInterval;// 攻撃間隔
		auto lockTime = TURN_LOCK_TIME;
		auto moveBackRange = owner->GetEnemyParam().fAttackRange;// これより近づいたら後退する
		auto moveSpeed = owner->GetEnemyParam().fMoveSpeed;// 後退速度

		// ターゲットへのベクトル情報
		VECTOR vToTarget = VSub(target->GetPos(), owner->GetPos());
		auto dist = VSize(vToTarget);// ターゲットまでの距離
		VECTOR vDirToTarget = VNorm(vToTarget);// ターゲット方向(正規化)
		VECTOR vDir = owner->GetDir();
		auto dot = VDot(vDir, vDirToTarget);// 内積

		// 視界外チェック
		{
			// 距離が離れすぎたら待機状態へ
			// 距離チェック
			bool bIsOutRange = (dist > owner->GetEnemyParam().fVisionRange * 1.2f);// 少し余裕を持たせる

			// 角度チェック
			auto limitCos = owner->GetEnemyParam().fVisionCos;// 視界のcos値

			// 内積がlimitCos未満なら扇の外
			bool bIsOutAngle = (dot < limitCos);

			// 離れすぎor視界外なら待機状態へ
			if (bIsOutRange || bIsOutAngle) {
				owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 念のため停止
				// 索敵結果をクリア
				owner->SetTargetDetected(false);
				return std::make_shared<Idle>();// 待機状態へ
			}
		}

		// 向きの制御
		{
			// 発射直前でなければターゲットの方向を向く
			// 滑らかに旋回させる
			if (_shotTimer < interval - lockTime) {
				// ターゲットの角度を計算
				auto targetAngle = atan2f(vToTarget.x, vToTarget.z);

				// 現在の角度を計算
				VECTOR vCurrentDir = owner->GetDir();
				auto currentAngle = atan2f(vCurrentDir.x, vCurrentDir.z);

				// 角度差を計算(-PI ~ +PI の範囲に収める)
				auto diffAngle = targetAngle - currentAngle;
				// 180度を超えないように補正
				while (diffAngle <= -DX_PI_F) diffAngle += DX_TWO_PI_F;
				while (diffAngle > DX_PI_F) diffAngle -= DX_TWO_PI_F;

				// 旋回速度の制限
				auto turnSpeedRad = owner->GetEnemyParam().fTurnSpeed * DEGREE_TO_RADIAN;// ラジアンに変換
				// 差が速度制限を超えていたら、速度分だけ回す
				if (diffAngle > turnSpeedRad) diffAngle = turnSpeedRad;
				else if (diffAngle < -turnSpeedRad) diffAngle = -turnSpeedRad;

				// 新しい向きベクトルを計算して設定
				auto newAngle = currentAngle + diffAngle;
				VECTOR vNewDir = VGet(sinf(newAngle), 0.0f, cosf(newAngle));
				owner->SetDir(vNewDir);
			}
			//else {
			//	// 発射直前は向きを固定する
			//}
		}

		// 移動の制御
		{
			VECTOR vMove = VGet(0.0f, 0.0f, 0.0f);

			// 距離が近い　かつ　プレイヤーの正面を向いている(45度)
			if (dist < moveBackRange && dot > 0.7f) {
				// 移動用にY成分を抜いたベクトルを作る
				VECTOR vToTargetXZ = vToTarget;
				vToTargetXZ.y = 0.0f;// 高さは無視する

				// XZ平面での方向を求める
				VECTOR vDirToTargetXZ = VNorm(vToTargetXZ);

				// ターゲットと逆方向のベクトルを作る
				VECTOR vBack = VScale(vDirToTargetXZ, -1.0f);

				// 後退速度を適用
				vMove = VScale(vBack, moveSpeed);
			}

			owner->SetMove(vMove);// 移動量を更新
		}

		// 発射判定
		if (_shotTimer >= interval && dot > 0.9f) {// ほぼ正面を向いているなら発射
			Shoot(owner);// 発射

			int playingHandle = EffectServer::GetInstance()->Play("Laser", owner->GetPos());// エフェクト再生
			if (playingHandle != -1)
			{
				// 敵の向きから角度を計算
				VECTOR vDir = owner->GetDir();
				auto yaw = atan2f(vDir.x, vDir.z) * RADIAN_TO_DEGREE;// Y軸回転角度(度)

				// エフェクトの回転を設定
				EffectServer::GetInstance()->SetRot(playingHandle, VGet(0.0f, yaw, 0.0f));
			}

			_shotTimer = 0.0f;// タイマーリセット
		}

		return nullptr;
	}

	void Attack::Shoot(Enemy* owner) {
		// 発射位置の計算
		VECTOR vMuzzlePos = VAdd(
			owner->GetPos(),
			VScale(owner->GetDir(), SHOOT_OFFSET_Z)// 少し前方にずらす
		);
		vMuzzlePos.y += SHOOT_OFFSET_Y;// 高さを合わせる

		// 弾を生成
		owner->SpawnBullet(vMuzzlePos, owner->GetDir(), BULLET_RADIUS, BULLET_SPEED, BULLET_LIFETIME);
	}
}
