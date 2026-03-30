#pragma once
#include "../Enemy.h"

// テンプレート関数の実装
template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostNoTarget(Enemy* owner)
{
	owner->SetTargetDetected(false);
	return std::make_shared<LostTargetState>();
}

template<typename IdleState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToIdleOutsideArea(Enemy* owner)
{
	if (owner->IsOutSideMoveArea())
	{
		return std::make_shared<IdleState>();
	}

	return nullptr;
}

template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostOutsideArea(Enemy* owner)
{
	if (owner->IsOutSideMoveArea())
	{
		owner->SetTargetDetected(false);
		return std::make_shared<LostTargetState>();
	}

	return nullptr;
}