// 担当 : 成田

#include "PlayerAbilityBase.h"

PlayerAbilityBase::PlayerAbilityBase()
{
	// 能力タイプ初期化
	_eAbilityType = ABILITY_TYPE::NONE;
}

PlayerAbilityBase::~PlayerAbilityBase()
{

}

bool PlayerAbilityBase::Initialize()
{
	return true;
}

bool PlayerAbilityBase::Terminate()
{
	return true;
}

bool PlayerAbilityBase::Process()
{
	return true;
}

bool PlayerAbilityBase::Render()
{
	return true;
}