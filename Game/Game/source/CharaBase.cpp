#include "CharaBase.h"

CharaBase::CharaBase()
{
	// 当たり判定用初期化
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_fCollisionR = 0.0f;

	// 基礎ステータス初期化
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fLife = 0.0f;
	_fGravity = 0.0f;

	// キャラタイプ
	_eCharaType = CHARA_TYPE::NONE;
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

void CharaBase::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType)
{
	if (_fLife <= 0.0f) return;	// 体力が0なら無効

	_fLife -= fDamage;

	if (_fLife < 0.0f) _fLife = 0.0f;	// 体力がマイナスにならないようにする
}