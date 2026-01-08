// 担当 : 成田

#include "SurfacePlayer.h"
#include "ApplicationMain.h"

namespace
{
	const float GRAVITY = -0.6f;	// 重力加速度
}

SurfacePlayer::SurfacePlayer()
{
	// モデル表示関係
	_iHandle = MV1LoadModel("res/base/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 1, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	// 位置の初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);

	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fLife = 50.0f;
	_fGravity = GRAVITY;

	// 固有変数の初期化
	_fVelY = 0.0f;
	_bIsJumping = false;
	_bIsStanding = true;
	_bIsCrouching = false;
	_bIsStartCrouch = false;

	// カプセルコリジョンの設定
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);	// 上端
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);	// 下端
	_fCollisionR = 20.f;						// 半径

	// 腰位置の設定
	_colSubY = 40.f;
	_bViewCollision = false;

	// 表示用オフセット
	_iDrawSizeOffset = 16;
	_iDrawOffsetX = 0;
	_iDrawOffsetY = 0;

	// 攻撃システム初期化
	_bCanCombo = false;
	_iComboCount = 0;

	// キャラタイプ
	_eCharType = CHARA_TYPE::SURFACE_PLAYER;

	// 攻撃データの初期化
	InitializeAttackData();
}

SurfacePlayer::~SurfacePlayer()
{
	MV1DeleteModel(_iHandle);
}

bool SurfacePlayer::Initialize()
{
	// 攻撃データの初期化
	InitializeAttackData();

	return true;
}

bool SurfacePlayer::Terminate()
{
	return true;
}

bool SurfacePlayer::Process()
{
	// 死亡処理
	ProcessDeath();

	// プレイヤーが死亡しているなら
	if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// Process呼び出し用関数
	ProcessCall();

	// 攻撃Process呼び出し用関数
	ProcessAttackCall();

	return true;
}

bool SurfacePlayer::Render()
{
	// プレイヤーが死亡しているなら
	//if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// モデル表示
	DrawModel();

	// デバッグ用
	//DrawDebug();

	// デバッグ用コリジョン表示
		// カプセル表示
		DrawCapsule3D(_vCollisionTop,_vCollisionBottom, _fCollisionR,16, GetColor(255, 255, 0),GetColor(255, 255, 0), TRUE);

	return true;
}