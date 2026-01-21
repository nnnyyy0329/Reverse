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

	// 攻撃システム初期化
	_vAttackColTop = VGet(0.0f, 0.0f, 0.0f);
	_vAttackColBottom = VGet(0.0f, 0.0f, 0.0f);
	_fAttackColR = 0.0f;
	_bCanCombo = false;
	_iComboCount = 0;

	// 回避関係
	_fDodgeMoveSpeed = 0.0f;	// 回避時の強制移動量
}

PlayerBase::~PlayerBase()
{

}

bool PlayerBase::Initialize()
{
	// 攻撃データの初期化を追加
	InitializeAttackData();

	return true;
}

bool PlayerBase::Terminate()
{
	return true;
}

bool PlayerBase::Process()
{
	// 攻撃関係Process呼び出し用関数
	CallProcessAttack();

	return true;
}

bool PlayerBase::Render()
{
	return true;
}
