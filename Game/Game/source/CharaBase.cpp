#include "CharaBase.h"

CharaBase::CharaBase()
{
	// 当たり判定用初期化
	_vCollisionTop = VGet(0.0f, 1.0f, 0.0f);
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_fCollisionR = 0.5f;

	// 基礎ステータス初期化
	_fMoveSpeed = 0.1f;
	_fDirSpeed = 5.0f;
	_fLife = 100.0f;
	_fGravity = -0.01f;
}

CharaBase::~CharaBase()
{

}

bool CharaBase::Initialize()
{
	return true;
}

bool CharaBase::Terminate()
{
	return true;
}

bool CharaBase::Process()
{
	return true;
}

bool CharaBase::Render()
{
	return true;
}