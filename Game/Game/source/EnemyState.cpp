#include "EnemyState.h"
#include "Enemy.h"

float EnemyState::CalcOffsetTime(Enemy* owner, float baseTime)
{
	if (!owner) { return baseTime; }
	return baseTime + owner->GetStateTimerOffset();
}