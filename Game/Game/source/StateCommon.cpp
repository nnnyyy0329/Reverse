#include "StateCommon.h"
#include "Enemy.h"

namespace 
{
	// ステート時間
	constexpr auto DEAD_TIME = 60.0f;// 死亡時間
	constexpr auto DOWN_TIME = 90.0f;// ダウン時間

	// ノックバック制御
	constexpr auto DAMAGE_KNOCKBACK_SPEED = 5.0f;// ノックバック速度
	constexpr auto DAMAGE_KNOCKBACK_TIME = 15.0f;// ノックバック時間
	constexpr auto DOWN_KNOCKBACK_SPEED = 18.0f;// ダウンノックバック速度
	constexpr auto DOWN_KNOCKBACK_TIME = 90.0f;// ダウンノックバック時間
	constexpr auto KNOCKBACK_DECELERATION = 0.95f;// ノックバック減速率

	// 判定閾値
	constexpr auto KNOCKBACK_MIN_DISTANCE = 0.001f;// ノックバック方向計算の最小距離

	constexpr auto BLEND_FRAME = 10.0f;// アニメーションブレンドフレーム数

	// 被ダメージ
	constexpr auto DAMAGE_ANIM_LENGTH = 42.0f;// ダメージアニメーションの長さ
	// コンボ数ごとに再生スピードを遅くする
	constexpr float DAMAGE_ANIM_SPEEDS[] = { 2.0f, 1.5f, 1.0f, 0.5f };
	// コンボ数から再生スピードを取得
	float GetDamageAnimSpeed(int comboCnt)
	{
		int index = comboCnt - 1;// コンボは1から始まるため
		if (index < 0) { index = 0; }
		if (index >= 4) { index = 3; }
		return DAMAGE_ANIM_SPEEDS[index];
	}
	// コンボ数からステート時間を取得
	float GetDamageStateTime(int comboCnt)
	{
		// アニメージョンの長さを再生スピードで割った値を返す
		return DAMAGE_ANIM_LENGTH / GetDamageAnimSpeed(comboCnt);
	}
}

namespace Common
{
	// 被ダメージ
	void Damage::Enter(Enemy* owner) 
	{
		// Enter時点のコンボ数を保存
		_comboCnt = owner->GetDamageComboCnt();

		// タイマー初期化
		_fTimer = 0.0f;
		_fKnockbackSpeed = DAMAGE_KNOCKBACK_SPEED;

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
		owner->GetAnimManager()->ChangeAnimationByName(param.animDamage, BLEND_FRAME, 1, GetDamageAnimSpeed(_comboCnt));
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
		if (_fTimer >= GetDamageStateTime(_comboCnt))
		{
			return owner->GetAfterDamageStateSelector(_comboCnt);
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
		SoundServer::GetInstance()->Play("SE_En_Down", DX_PLAYTYPE_BACK);
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
			return owner->GetAfterDownStateSelector();
		}

		return nullptr;
	}
}
