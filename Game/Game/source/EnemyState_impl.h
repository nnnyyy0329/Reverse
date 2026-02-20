#pragma once
#include "Enemy.h"

// テンプレート関数の実装
template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostNoTarget(Enemy* owner)
{
	owner->SetTargetDetected(false);
	return std::make_shared<LostTargetState>();
}

template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostOverChaseLimit(Enemy* owner, float fDist)
{
	const auto& param = owner->GetEnemyParam();
	if (fDist > param.fChaseLimitRange)
	{
		owner->SetTargetDetected(false);
		return std::make_shared<LostTargetState>();
	}

	return nullptr;
}


template<typename IdleState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToIdleOutsideArea(Enemy* owner)
{
	if (!owner->CheckInsideMoveArea(owner->GetPos()))
	{
		return std::make_shared<IdleState>();
	}

	return nullptr;
}

template<typename LostTargetState>
inline std::shared_ptr<EnemyState> EnemyState::TransitionToLostOutsideArea(Enemy* owner)
{
	if (!owner->CheckInsideMoveArea(owner->GetPos()))
	{
		owner->SetTargetDetected(false);
		return std::make_shared<LostTargetState>();
	}

	return nullptr;
}