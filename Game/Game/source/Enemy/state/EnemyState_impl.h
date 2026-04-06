#pragma once
#include "../Enemy.h"

// テンプレート関数の実装
template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostNoTarget(Enemy* owner)
{
	owner->SetTargetDetected(false);
	return owner->GetState<LostTargetState>();
}

template<typename IdleState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToIdleOutsideArea(Enemy* owner)
{
	if (owner->IsOutSideMoveArea())
	{
		return owner->GetState<IdleState>();
	}

	return nullptr;
}

template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostOutsideArea(Enemy* owner)
{
	if (owner->IsOutSideMoveArea())
	{
		owner->SetTargetDetected(false);
		return owner->GetState<LostTargetState>();
	}

	return nullptr;
}