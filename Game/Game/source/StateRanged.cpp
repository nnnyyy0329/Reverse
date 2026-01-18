#include "StateRanged.h"
#include "Enemy.h"

namespace {
	constexpr auto SHOOT_OFFSET_Z = 20.0f;// 発射位置前方オフセット
	constexpr auto SHOOT_OFFSET_Y = 40.0f;// 発射位置高さオフセット

	constexpr auto TURN_LOCK_TIME = 10.0f;// 発射の何フレーム前になったら向きを固定するか

	constexpr auto BULLET_RADIUS = 10.0f;// 弾の当たり判定半径
	constexpr auto BULLET_SPEED = 15.0f;// 弾の速度
	constexpr auto BULLET_LIFETIME = 120.0f;// 弾の寿命(フレーム数)
}

namespace Ranged
{
	// 待機
	void Idle::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Idle::Update(Enemy* owner) {
		// 索敵チェック
		auto target = owner->GetTarget();// ターゲット取得
		if (owner->IsTargetVisible(target))// 視界内なら
		{
			return std::make_shared<Detect>();// 発見状態へ
		}

		// 待機中に見渡すなどの動きを入れたいならここに追加

		return nullptr;
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
		if (!target) return std::make_shared<Idle>();// ターゲットがいない場合は待機状態へ

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
			_shotTimer = 0.0f;// タイマーリセット
		}

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
				return std::make_shared<Idle>();// 待機状態へ
			}
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
