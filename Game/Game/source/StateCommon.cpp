#include "StateCommon.h"
#include "Enemy.h"

namespace 
{
	constexpr auto DOWN_TIME = 180.0f;// 攻撃を受けた後のダウン時間
	constexpr auto STUN_TIME = 180.0f;// スタン時間
	constexpr auto DEAD_TIME = 60.0f;// 死亡アニメーション時間

	constexpr auto KNOCKBACK_SPEED = 5.0f;// ノックバック速度
	constexpr auto KNOCKBACK_TIME = 15.0f;// ノックバック時間
	constexpr auto KNOCKBACK_DECELERATION = 0.9f;// ノックバック減速率
}

namespace Common
{
	// 被ダメージ
	void Damage::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Damage::Update(Enemy* owner) 
	{
		_fTimer++;

		// 被ダメージ回数によってステートを変える
		if(owner->GetDamageCount() >= owner->GetEnemyParam().damageToDown)
		{
			// ダウン状態へ移行
			return std::make_shared<Down>();
		}
		// 次のステートはオーナーに任せる
		return owner->GetRecoveryState();

		return nullptr;
	}





	// 死亡
	void Dead::Enter(Enemy* owner)
	{
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 死亡時は移動しないようにする
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Dead::Update(Enemy* owner)
	{
		_fTimer++;

		// 時間経過で
		if (_fTimer >= DEAD_TIME) 
		{
			// オブジェクト削除可能にする
			owner->EnableRemove();
		}

		return nullptr;
	}





	// スタン
	void Stun::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// スタン中は移動しないようにする

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Stun::Update(Enemy* owner)
	{
		_fTimer++;
		// 時間経過で
		if (_fTimer >= STUN_TIME) 
		{
			// 次のステートはオーナーに任せる
			return owner->GetRecoveryState();
		}
		return nullptr;
	}





	// ダウン
	void Down::Enter(Enemy* owner) 
	{
		_fTimer = 0.0f;
		_fKnockbackSpeed = KNOCKBACK_SPEED;
		owner->ResetDamageCount();// 被ダメージ回数リセット

		// ノックバック方向を計算
		auto target = owner->GetTarget();
		if (target)
		{
			// ターゲットから敵への方向ベクトル(XZ平面のみ)
			VECTOR vToEnemy = VSub(owner->GetPos(), target->GetPos());
			vToEnemy.y = 0.0f;// Y成分は無視

			// 正規化してノックバック方向とする
			float distance = VSize(vToEnemy);
			if (distance > 0.001f)
			{
				_vKnockbackDir = VScale(vToEnemy, 1.0f / distance);
			}
			else 
			{
				// ターゲットと完全にかっさなっている場合は敵の向きの逆方向
				_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
			}
		}
		else
		{
			// ターゲットがいない場合は敵の向きの逆方向
			_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
		}

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Down::Update(Enemy* owner)
	{
		_fTimer++;

		// ノックバック処理
		if (_fTimer < KNOCKBACK_TIME)
		{
			// ノックバック速度を徐々に適用
			_fKnockbackSpeed *= KNOCKBACK_DECELERATION;

			// ノックバック方向に移動
			VECTOR vMove = VScale(_vKnockbackDir, _fKnockbackSpeed);
			owner->SetMove(vMove);
		}
		else
		{
			// ノックバック終了後は停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
		}

		// 時間経過で
		if (_fTimer >= DOWN_TIME) {
			// 次のステートはオーナーに任せる
			return owner->GetRecoveryState();
		}

		return nullptr;
	}
}
