#include "StateCommon.h"
#include "Enemy.h"

namespace {
	constexpr auto DAMAGE_STUN_TIME = 30.0f;// ダメージを受けてからの硬直時間
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

		// 時間経過で
		if (_fTimer >= DAMAGE_STUN_TIME) {
			// 次のステートはオーナーに任せる
			return owner->GetRecoveryState();
		}

		return nullptr;
	}
}
