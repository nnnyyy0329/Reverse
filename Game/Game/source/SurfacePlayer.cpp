// 担当 : 成田

#include "SurfacePlayer.h"
#include "ApplicationMain.h"

namespace
{
	const float GRAVITY = -0.6f;			// 重力加速度
	const float DEFAULT_LIFE = 50.0f;		// デフォルト体力

	const int DRAW_SIZE_OFFSET = 16;	// 描画サイズオフセット
	const int DRAW_OFFSET_X = 900;		// 描画Xオフセット
	const int DRAW_OFFSET_Y = 0;		// 描画Yオフセット
}

SurfacePlayer::SurfacePlayer()
{
	// モデル読み込み
	int modelHandle = ResourceServer::GetInstance()->GetHandle("SurfacePlayer");

	// AnimManagerにモデルハンドルを複製して設定
	int duplicatedHandle = MV1DuplicateModel(modelHandle);
	_animManager.SetModelHandle(duplicatedHandle);

	// 初期アニメーションの設定

	// 位置の初期化
	_vPos = VGet(230.0f,100.0f, 0.0f);
	_vDir = VGet(0, 0, -1);

	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;	// 状態
	_fMoveSpeed = 0.0f;						// 移動速度
	_fDirSpeed = 0.0f;						// 回転速度
	_fLife = DEFAULT_LIFE;					// 体力
	_fGravity = GRAVITY;					// 重力

	// アクション関係変数の初期化
	_fVelY = 0.0f;				// Y方向の速度
	_bIsJumping = false;		// ジャンプ中かどうか
	_bIsStanding = true;		// 着地しているかどうか
	_bIsCrouching = false;		// しゃがんでいるかどうか
	_bIsStartCrouch = false;	// しゃがみ開始フラグ

	// カプセルコリジョンの設定
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);	// 上端
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);	// 下端
	_fCollisionR = 20.f;						// 半径

	// 腰位置の設定
	_colSubY = 40.f;
	_bViewCollision = false;

	// 表示用オフセット
	_iDrawSizeOffset = DRAW_SIZE_OFFSET;	// 描画サイズオフセット
	_iDrawOffsetX = DRAW_OFFSET_X;			// 描画Xオフセット
	_iDrawOffsetY = DRAW_OFFSET_Y;			// 描画Yオフセット

	// 攻撃システム初期化
	_bCanCombo = false;
	_iComboCount = 0;

	// キャラタイプ
	SetCharaType(CHARA_TYPE::SURFACE_PLAYER);

	//// 攻撃データの初期化
	//InitializeAttackData();
}

SurfacePlayer::~SurfacePlayer()
{
}

bool SurfacePlayer::Initialize()
{
	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fGravity = GRAVITY;

	// 攻撃システム初期化
	_bCanCombo = false;
	_iComboCount = 0;

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

	// アクション関係Process呼び出し用関数
	CallProcess();

	// 攻撃関係Process呼び出し用関数
	CallProcessAttack();

	CharaBase::Process();

	return true;
}

bool SurfacePlayer::Render()
{
	CharaBase::Render();

	// プレイヤーが死亡しているなら
	//if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	//// モデル表示
	//DrawModel();

	// デバッグ用
	CallDraw();

	return true;
}

// 被ダメージ処理
void SurfacePlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType)
{
	CharaBase::ApplyDamage(fDamage, eType);
}

// 攻撃判定のパラメーター
AttackConstants SurfacePlayer::GetAttackConstants()
{
	// SurfacePlayer専用の攻撃定数
	AttackConstants constants;

	constants.ATTACK_OFFSET_SCALE = 75.0f;	// 攻撃判定オフセット倍率	
	constants.COMMON_RADIUS = 30.0f;		// 半径	
	constants.COMMON_DELAY = 10.0f;			// 発生フレーム
	constants.COMMON_DURATION = 15.0f;		// 持続フレーム	
	constants.COMMON_RECOVERY = 20.0f;		// 硬直フレーム
	constants.NORMAL_DAMAGE = 5.0f;			// 通常ダメージ
	constants.FINISHER_DAMAGE = 20.0f;		// フィニッシャーダメージ
	constants.SURFACE_MAX_COMBO_COUNT = 3;	// 表プレイヤー用コンボカウント

	return constants;
}

// 攻撃判定の大きさ設定
void SurfacePlayer::GetAttackConfigs(AttackConfig configs[3])
{
	// SurfacePlayer専用の攻撃設定
	configs[0] = { {0.0f, 100.0f, 0.0f}, {0.0f, 50.0f, 0.0f}, 5.0f };	// 第1攻撃
	configs[1] = { {0.0f, 250.0f, 0.0f}, {0.0f, 150.0f, 0.0f}, 5.0f };	// 第2攻撃
	configs[2] = { {0.0f, 200.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 20.0f };	// 第3攻撃
}