// 担当 : 成田

#include "AbilityBase.h"

AbilityBase::AbilityBase()
{
	// 能力タイプ初期化
	_eAbilityType = ABILITY_TYPE::NONE;
}

AbilityBase::~AbilityBase()
{

}

bool AbilityBase::Initialize()
{
	return true;
}

bool AbilityBase::Terminate()
{
	return true;
}

bool AbilityBase::Process()
{
	return true;
}

bool AbilityBase::Render()
{
	return true;
}