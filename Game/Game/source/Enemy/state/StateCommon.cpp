#include "StateCommon.h"
#include "../Enemy.h"

namespace 
{
	// ステート時間
	constexpr auto DEAD_TIME = 60.0f;// 死亡時間
	constexpr auto DOWN_TIME =120.0f;// ダウン時間

	// ノックバック制御
	constexpr auto DAMAGE_KNOCKBACK_SPEED = 7.0f;// ノックバック速度
	constexpr auto DAMAGE_KNOCKBACK_TIME = 12.0f;// ノックバック時間
	constexpr auto DOWN_KNOCKBACK_SPEED = 18.0f;// ダウンノックバック速度
	constexpr auto DOWN_KNOCKBACK_TIME = 75.0f;// ダウンノックバック時間
	constexpr auto KNOCKBACK_DECELERATION = 0.95f;// ノックバック減速率

	// 判定閾値
	constexpr auto KNOCKBACK_MIN_DISTANCE = 0.001f;// ノックバック方向計算の最小距離

	constexpr auto BLEND_FRAME = 10.0f;// アニメーションブレンドフレーム数
}

namespace Common
{
	// 被ダメージ
	void Damage::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fKnockbackSpeed = DAMAGE_KNOCKBACK_SPEED;
		_param = owner->GetEnemyParam();

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
		owner->GetAnimManager()->ChangeAnimationByName(_param.animDamage, BLEND_FRAME, 1, _param.fDamageAnimSpeed);
	}

	std::shared_ptr<EnemyState> Damage::Update(Enemy* owner) 
	{
		// タイマー更新
		_fTimer++;

		// ノックバック処理
		if (_fTimer < DAMAGE_KNOCKBACK_TIME)
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

		// コンボ数に応じたステート時間
		if (_fTimer >= owner->GetEnemyParam().fDamageTime)
		{
			return owner->GetAfterDamageStateSelector();
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
		owner->GetAnimManager()->ChangeAnimationByName(param.animDead, BLEND_FRAME, 1);

		// SE
		SoundServer::GetInstance()->Play("SE_En_Dead", DX_PLAYTYPE_BACK);
	}

	std::shared_ptr<EnemyState> Dead::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// 死亡時間経過チェック
		if (_fTimer >= DEAD_TIME) 
		{
			// エフェクト
			EffectServer::GetInstance()->Play("En_Dead02", owner->GetPos());

			owner->EnableRemove();// オブジェクト削除可能設定
		}

		return nullptr;
	}





	// ダウン
	void Down::Enter(Enemy* owner) 
	{
		// タイマー初期化
		_fTimer = 0.0f;
		_fKnockbackSpeed = DOWN_KNOCKBACK_SPEED;

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
		owner->GetAnimManager()->ChangeAnimationByName(param.animDown, BLEND_FRAME, 1);

		// SE
		if (param.bDownSE)
		{
			SoundServer::GetInstance()->Play("SE_En_Down", DX_PLAYTYPE_BACK);
		}
	}

	std::shared_ptr<EnemyState> Down::Update(Enemy* owner)
	{
		// タイマー更新
		_fTimer++;

		// ノックバック処理
		if (_fTimer < DOWN_KNOCKBACK_TIME)
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
			// 死亡チェック
			if(owner->IsDead())
			{
				// エフェクト
				EffectServer::GetInstance()->Play("En_Dead02", owner->GetPos());

				owner->EnableRemove();// オブジェクト削除可能設定
			}

			return owner->GetAfterDownStateSelector();
		}

		return nullptr;
	}
}
