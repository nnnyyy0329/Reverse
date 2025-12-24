#include "PlayerBase.h"

PlayerBase::PlayerBase()
{
	// ƒLƒƒƒ‰‚Ìó‘Ô‰Šú‰»
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_eOldPlayerStatus = PLAYER_STATUS::NONE;
}

PlayerBase::~PlayerBase()
{

}

bool PlayerBase::Initialize()
{
	return true;
}

bool PlayerBase::Terminate()
{
	return true;
}

bool PlayerBase::Process()
{
	return true;
}

bool PlayerBase::Render()
{
	return true;
}
