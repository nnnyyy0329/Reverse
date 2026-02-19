#pragma once
#include "Enemy.h"

// テンプレート関数の実装
template<typename IdleState>
inline std::shared_ptr<EnemyState> EnemyState::HandleNoTarget(Enemy* owner)
{
	owner->SetTargetDetected(false);
	return std::make_shared<IdleState>();
}

template<typename IdleState>
inline std::shared_ptr<EnemyState> EnemyState::CheckChaseLimitAndHandle(Enemy* owner, float fDist)
{
	const auto& param = owner->GetEnemyParam();
	if (fDist > param.fChaseLimitRange)
	{
		owner->SetTargetDetected(false);
		return std::make_shared<IdleState>();
	}

	return nullptr;
}
