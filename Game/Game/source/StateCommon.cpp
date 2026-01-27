#include "StateCommon.h"
#include "Enemy.h"

namespace {
	constexpr auto DOWN_TIME = 60.0f;// 攻撃を受けた後のダウン時間
	constexpr auto STUN_TIME = 120.0f;// スタン時間
	constexpr auto DEAD_TIME = 60.0f;// 死亡アニメーション時間
}

namespace Common
{
	// 被ダメージ
	void Damage::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Damage::Update(Enemy* owner) {
		_fTimer++;

		// 被ダメージ回数によってステートを変える
		if(owner->GetDamageCount() >= owner->GetEnemyParam().damageToDown){
			// ダウン状態へ移行
			return std::make_shared<Down>();
		}
		// 次のステートはオーナーに任せる
		return owner->GetRecoveryState();

		return nullptr;
	}

	// 死亡
	void Dead::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// 死亡時は移動しないようにする
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Dead::Update(Enemy* owner) {
		_fTimer++;

		// 時間経過で
		if (_fTimer >= DEAD_TIME) {
			// オブジェクト削除可能にする
			owner->EnableRemove();
		}

		return nullptr;
	}

	// スタン
	void Stun::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// スタン中は移動しないようにする
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Stun::Update(Enemy* owner) {
		_fTimer++;
		// 時間経過で
		if (_fTimer >= STUN_TIME) {
			// 次のステートはオーナーに任せる
			return owner->GetRecoveryState();
		}
		return nullptr;
	}

	// ダウン
	void Down::Enter(Enemy* owner) {
		_fTimer = 0.0f;
		owner->SetMove(VGet(0.0f, 0.0f, 0.0f));// ダウン中は移動しないようにする
		owner->ResetDamageCount();// 被ダメージ回数リセット
		// ここでアニメーション設定
	}

	std::shared_ptr<EnemyState> Down::Update(Enemy* owner) {
		_fTimer++;
		// 時間経過で
		if (_fTimer >= DOWN_TIME) {
			// 次のステートはオーナーに任せる
			return owner->GetRecoveryState();
		}
		return nullptr;
	}
}
