#include "GameObjectBase.h"

GameObjectBase::GameObjectBase()
{
	// 位置関連初期化
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vOldPos = VGet(0.0f, 0.0f, 0.0f);
	_vDir = VGet(0.0f, 0.0f, 0.0f);
	_vScale = VGet(1.0f, 1.0f, 1.0f);
	_vMove = VGet(0.0f, 0.0f, 0.0f);

	// モデル関連初期化
	_iHandle = -1;
	_iAttachIndex = -1;
	_fTotalTime = 0.0f;
	_fPlayTime = 0.0f;
}

GameObjectBase::~GameObjectBase()
{

}

bool GameObjectBase::Initialize()
{
	return true;
}

bool GameObjectBase::Terminate()
{
	return true;
}

bool GameObjectBase::Process()
{
	return true;
}

bool GameObjectBase::Render()
{
	return true;
}