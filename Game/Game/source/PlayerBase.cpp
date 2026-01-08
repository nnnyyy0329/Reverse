// 担当 : 成田

#include "PlayerBase.h"

PlayerBase::PlayerBase()
{
	// キャラの状態初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_eOldPlayerStatus = PLAYER_STATUS::NONE;

	// 腰位置の設定
	_colSubY = 0.0f;
	_bViewCollision = false;

	// アクション関係変数の初期化
	_fVelY = 0.0f;
	_bIsJumping = false;
	_bIsStanding = true;
	_bIsCrouching = false;
	_bIsStartCrouch = false;

	// 表示用オフセット
	_iDrawSizeOffset = 0;
	_iDrawOffsetX = 0;
	_iDrawOffsetY = 0;
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
