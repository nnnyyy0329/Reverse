#include "PlayerAttackSystem.h"
#include "CharaBase.h"

PlayerAttackSystem::PlayerAttackSystem()
{
	_attackMovement.moveDir = VGet(0.0f, 0.0f, 0.0f);
	_attackMovement.moveDistance = 0.0f;
	_attackMovement.moveSpeed = 0.0f;
	_attackMovement.decayRate = 0.0f;
	_attackMovement.canMove = false;
}

PlayerAttackSystem::~PlayerAttackSystem()
{

}

// ˆÚ“®ŒvZˆ—
AttackMovement PlayerAttackSystem::CalculateMovement(ATTACK_STATE state, std::shared_ptr<CharaBase> owner)
{
	AttackMovement movement;
	movement.canMove = false;

	switch(state)
	{
		case ATTACK_STATE::STARTUP:
		{
			break;
		}

		case ATTACK_STATE::ACTIVE:
		{
			break;
		}

		case ATTACK_STATE::RECOVERY:
		{
			break;
		}

		default:
			break;
	}

	// UŒ‚ˆÚ“®î•ñ‚ğİ’è
	return movement;
}
