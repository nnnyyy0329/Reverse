#include "DodgeSystem.h"

DodgeSystem::DodgeSystem()
{
	_eDodgeState = DODGE_STATE::NONE;

	_bIsDodging = false;
	_bIsInvincible = false;

	// 回避設定初期化
	InitializeConfig();
}

DodgeSystem::~DodgeSystem()
{

}

bool DodgeSystem::InitializeConfig()
{
	_stcDodgeConfig.dodgeDuration = 0.0f;
	_stcDodgeConfig.invincibleDuration = 0.0f;
	_stcDodgeConfig.dodgeMoveSpeed = 0.0f;

	return true;
}

bool DodgeSystem::Terminate()
{
	return true;
}

bool DodgeSystem::Process()
{
	return true;
}

bool DodgeSystem::Render()
{
	return true;
}

// 回避開始
void DodgeSystem::StartDodge()
{

}

// 回避中かチェック
bool DodgeSystem::IsDodging() const
{
	// 回避中フラグが立っているかチェック
	if(_bIsDodging)
	{
		// 回避中
		return true;
	}

	return false;
}

// 無敵状態かチェック
bool DodgeSystem::IsInvincible() const
{
	// 無敵状態フラグが立っているかチェック
	if(_bIsInvincible)
	{
		// 無敵状態
		return true;
	}

	return false;
}