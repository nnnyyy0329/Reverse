#include "StateCommon.h"
#include "Enemy.h"

namespace 
{
	// 状態時間
	constexpr auto DAMAGE_TIME = 50.0f;// 被ダメージ時間
	constexpr auto STUN_TIME = 180.0f;// スタン時間
	constexpr auto DEAD_TIME = 60.0f;// 死亡時間
	constexpr auto DOWN_TIME = 90.0f;// ダウン時間

	// ノックバック制御
	constexpr auto KNOCKBACK_SPEED = 5.0f;// ノックバック速度
	constexpr auto KNOCKBACK_TIME = 15.0f;// ノックバック時間
	constexpr auto KNOCKBACK_DECELERATION = 0.9f;// ノックバック減速率

	// 判定閾値
	constexpr auto KNOCKBACK_MIN_DISTANCE = 0.001f;// ノックバック方向計算の最小距離
}

namespace Common
{
	// 被ダメージ
	void Damage::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fKnockbackSpeed = KNOCKBACK_SPEED;

		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		if (targetInfo.bExist)
		{
			// ターゲットから敵へのベクトル計算
			VECTOR vToEnemy = VScale(targetInfo.vToTarget, -1.0f);
			vToEnemy.y = 0.0f;

			float dist = VSize(vToEnemy);

			if (dist > KNOCKBACK_MIN_DISTANCE)
			{
				_vKnockbackDir = VScale(vToEnemy, 1.0f / dist);
			}
			else
			{
				_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
			}
		}
		else
		{
			_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
		}

		// ここでアニメーション設定
		// 敵の種類ごとのアニメーション名を取得
		const auto& param = owner->GetEnemyParam();
		owner->GetAnimManager()->ChangeAnimationByName(param.animDamage, 1.0f, 1);
	}

	std::shared_ptr<EnemyState> Damage::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// ダウン判定チェック
		if (owner->GetDamageCount() >= owner->GetEnemyParam().damageToDown)
		{
			return std::make_shared<Down>();// ダウン状態へ
		}

		// ノックバック処理
		if (_fTimer < KNOCKBACK_TIME)
		{
			// 速度減衰処理
			_fKnockbackSpeed *= KNOCKBACK_DECELERATION;

			MoveToTarget(owner, _vKnockbackDir, _fKnockbackSpeed);
		}
		else
		{
			// ノックバック終了後停止
			StopMove(owner);
		}

		// 被ダメージ時間経過チェック
		if (_fTimer >= DAMAGE_TIME) 
		{
			return owner->GetRecoveryState();// 回復状態へ
		}

		return nullptr;
	}





	// 死亡
	void Dead::Enter(Enemy* owner)
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 移動停止
		StopMove(owner);

		// ここでアニメーション設定
		const auto& param = owner->GetEnemyParam();
		owner->GetAnimManager()->ChangeAnimationByName(param.animDead, 1.0f, 1);
	}

	std::shared_ptr<EnemyState> Dead::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 死亡時間経過チェック
		if (_fTimer >= DEAD_TIME) 
		{
			owner->EnableRemove();// オブジェクト削除可能設定
		}

		return nullptr;
	}





	// スタン
	void Stun::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;

		// 移動停止
		StopMove(owner);

		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Stun::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// スタン時間経過チェック
		if (_fTimer >= STUN_TIME) 
		{
			return owner->GetRecoveryState();// 回復状態へ
		}

		return nullptr;
	}





	// ダウン
	void Down::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fKnockbackSpeed = KNOCKBACK_SPEED;

		// 被ダメージ回数リセット
		owner->ResetDamageCount();

		// ターゲット情報取得
		auto targetInfo = GetTargetInfo(owner);

		if (targetInfo.bExist)
		{
			// ターゲットから敵へのベクトル計算
			VECTOR vToEnemy = VScale(targetInfo.vToTarget, -1.0f);
			vToEnemy.y = 0.0f;

			float dist = VSize(vToEnemy);

			if (dist > KNOCKBACK_MIN_DISTANCE)
			{
				_vKnockbackDir = VScale(vToEnemy, 1.0f / dist);
			}
			else
			{
				_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
			}
		}
		else
		{
			_vKnockbackDir = VScale(owner->GetDir(), -1.0f);
		}

		// ここでアニメーション設定
		const auto& param = owner->GetEnemyParam();
		owner->GetAnimManager()->ChangeAnimationByName(param.animDown, 1.0f, 1);
	}

	std::shared_ptr<EnemyState> Down::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ノックバック処理
		if (_fTimer < KNOCKBACK_TIME)
		{
			// 速度減衰処理
			_fKnockbackSpeed *= KNOCKBACK_DECELERATION;

			MoveToTarget(owner, _vKnockbackDir, _fKnockbackSpeed);
		}
		else
		{
			// ノックバック終了後停止
			owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
		}

		// ダウン時間経過チェック
		if (_fTimer >= DOWN_TIME)
		{
			return owner->GetRecoveryState();// 回復状態へ
		}

		return nullptr;
	}
}
