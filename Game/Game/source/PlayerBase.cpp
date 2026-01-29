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

	// カメラ角度初期化
	_cameraAngle = 0.0f;
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

// カメラ角度に合わせて移動方向を変換する
VECTOR PlayerBase::TransformMoveDirection(VECTOR move, float cameraAngle)
{
	if(VSize(move) == 0.0f){ return move; }	// 移動量が0ならそのまま返す

	// カメラ角度に基づいて移動方向を変換
	float _cosAngle = cosf(cameraAngle);
	float _sinAngle = sinf(cameraAngle);

	VECTOR transformMove;

	// X軸とZ軸の変換
	transformMove.x = move.x * _cosAngle - move.z * _sinAngle;	// X軸方向
	transformMove.y = move.y;									// Y軸方向はそのまま
	transformMove.z = move.x * _sinAngle + move.z * _cosAngle;	// Z軸方向

	return transformMove;
}
