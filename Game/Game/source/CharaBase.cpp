#include "CharaBase.h"

CharaBase::CharaBase()
{
	// 当たり判定用初期化
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_fCollisionR = 0.0f;
	_fCollisionHeight = 0.0f;

	// 基礎ステータス初期化
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fLife = 0.0f;
	_fMaxLife = 0.0f;
	_fGravity = 0.0f;
	_bIsDead = false;

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
	GameObjectBase::Process();

	return true;
}

bool CharaBase::Render()
{
	GameObjectBase::Render();

	return true;
}

void CharaBase::DebugRender()
{
}

void CharaBase::CollisionRender()
{
	// カプセル当たり判定
	DrawCapsule3D(
		_vCollisionBottom,
		_vCollisionTop,
		_fCollisionR,
		8,
		GetColor(255, 0, 0),
		GetColor(0, 255, 0),
		FALSE
	);
}

// 被ダメージ処理
void CharaBase::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	if (_fLife <= 0.0f) return;	// 体力が0なら無効

	_fLife -= fDamage;

	if (_fLife < 0.0f) _fLife = 0.0f;	// 体力がマイナスにならないようにする
}

// 弾による被ダメージ処理
void CharaBase::ApplyDamageByBullet(float fDamage, CHARA_TYPE eType)
{
	if (_fLife <= 0.0f) return;	// 体力が0なら無効

	_fLife -= fDamage;

	if (_fLife < 0.0f) _fLife = 0.0f;	// 体力がマイナスにならないようにする
}
