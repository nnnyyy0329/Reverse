// 担当 : 成田

#include "InteriorPlayer.h"

namespace
{
	const float GRAVITY = -0.6f;	// 重力加速度
}

InteriorPlayer::InteriorPlayer()
{
	// モデル表示関係
	_iHandle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, -1, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	// 位置の初期化
	_vPos = VGet(100, 0, 0);
	_vDir = VGet(0, 0, -1);

	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fLife = 50.0f;
	_fGravity = GRAVITY;

	// アクション関係変数の初期化
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
	SetCharaType(CHARA_TYPE::PLAYER);
}

InteriorPlayer::~InteriorPlayer()
{
	MV1DeleteModel(_iHandle);
}

bool InteriorPlayer::Initialize()
{
	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fGravity = GRAVITY;

	// 攻撃システム初期化
	_bCanCombo = false;
	_iComboCount = 0;

	// 攻撃データの初期化を追加
	InitializeAttackData();

	return true;
}

bool InteriorPlayer::Terminate()
{
	return true;
}

bool InteriorPlayer::Process()
{
	// 死亡処理
	ProcessDeath();

	// プレイヤーが死亡しているなら
	if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// アクション関係Process呼び出し用関数
	CallProcess();

	// 攻撃関係Process呼び出し用関数
	CallProcessAttack();

	return true;
}

bool InteriorPlayer::Render()
{
	// プレイヤーが死亡しているなら
	//if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// モデル表示
	DrawModel();

	// デバッグ用
	CallDraw();

	return true;
}

// 攻撃判定のパラメーター
AttackConstants InteriorPlayer::GetAttackConstants()
{
	// SurfacePlayer専用の攻撃定数
	AttackConstants constants;

	constants.ATTACK_OFFSET_SCALE = 60.0f;	
	constants.COMMON_RADIUS = 25.0f;		
	constants.COMMON_DELAY = 8.0f;			  
	constants.COMMON_DURATION = 12.0f;		
	constants.COMMON_RECOVERY = 18.0f;		
	constants.NORMAL_DAMAGE = 4.0f;			
	constants.FINISHER_DAMAGE = 15.0f;		
	constants.MAX_COMBO_COUNT = 4;			

	return constants;
}

// 攻撃判定の大きさ設定
void InteriorPlayer::GetAttackConfigs(AttackConfig configs[3])
{
	// SurfacePlayer専用の攻撃設定
	configs[0] = {{0.0f, 80.0f, 40.0f}, {0.0f, 120.0f, 40.0f}, 4.0f};	// 第1攻撃
	configs[1] = {{0.0f, 20.0f, 30.0f}, {0.0f, 100.0f, 30.0f}, 4.0f};	// 第2攻撃
	configs[2] = {{0.0f, 150.0f, 35.0f}, {0.0f, 80.0f, 35.0f}, 15.0f};	// 第3攻撃
}