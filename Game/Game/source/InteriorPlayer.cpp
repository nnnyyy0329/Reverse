// 担当 : 成田

#include "InteriorPlayer.h"

namespace
{
	constexpr float GRAVITY = -0.6f;		// 重力加速度
	const float DEFAULT_LIFE = 50.0f;		// デフォルト体力

	constexpr int DRAW_SIZE_OFFSET = 16;	// 描画サイズオフセット
	constexpr int DRAW_OFFSET_X = 900;		// 描画Xオフセット
	constexpr int DRAW_OFFSET_Y = 0;		// 描画Yオフセット
}

InteriorPlayer::InteriorPlayer()
{
	// モデル読み込み
	int modelHandle = ResourceServer::GetInstance()->GetHandle("InteriorPlayer");

	// AnimManagerにモデルハンドルを複製して設定
	int duplicatedHandle = MV1DuplicateModel(modelHandle);
	_animManager.SetModelHandle(duplicatedHandle);

	// 初期アニメーションの設定

	// 位置の初期化
	_vPos = VGet(100, 0, 0);
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
	_iDrawSizeOffset = DRAW_SIZE_OFFSET;	// ずらす大きさ
	_iDrawOffsetX = DRAW_OFFSET_X;			// 描画Xオフセット
	_iDrawOffsetY = DRAW_OFFSET_Y;			// 描画Yオフセット

	// 攻撃システム初期化
	_bCanCombo = false;
	_iComboCount = 0;

	// キャラタイプ
	SetCharaType(CHARA_TYPE::INTERIOR_PLAYER);
}

InteriorPlayer::~InteriorPlayer()
{
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

	CharaBase::Process();

	return true;
}

bool InteriorPlayer::Render()
{
	CharaBase::Render();

	// プレイヤーが死亡しているなら
	//if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// モデル表示
	DrawModel();

	// デバッグ用
	CallDraw();

	return true;
}

// 被ダメージ処理
void InteriorPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType)
{
	CharaBase::ApplyDamage(fDamage, eType);
}

// 攻撃判定のパラメーター
AttackConstants InteriorPlayer::GetAttackConstants()
{
	// SurfacePlayer専用の攻撃定数
	AttackConstants constants;

	constants.ATTACK_OFFSET_SCALE = 60.0f;	// 攻撃判定オフセット倍率	
	constants.COMMON_RADIUS = 25.0f;		// 半径
	constants.COMMON_DELAY = 8.0f;			// 発生フレーム
	constants.COMMON_DURATION = 12.0f;		// 持続フレーム
	constants.COMMON_RECOVERY = 18.0f;		// 硬直フレーム
	constants.NORMAL_DAMAGE = 4.0f;			// 通常ダメージ
	constants.FINISHER_DAMAGE = 15.0f;		// フィニッシャーダメージ
	constants.INTERIOR_MAX_COMBO_COUNT = 5;	// 裏プレイヤー用コンボカウント

	return constants;
}

// 攻撃判定の大きさ設定
void InteriorPlayer::GetAttackConfigs(AttackConfig configs[5])
{
	// SurfacePlayer専用の攻撃設定
	configs[0] = { {0.0f, 80.0f, 0.0f}, {0.0f, 120.0f, 0.0f}, 4.0f };	// 第1攻撃
	configs[1] = { {0.0f, 20.0f, 0.0f}, {0.0f, 100.0f, 0.0f}, 4.0f };	// 第2攻撃
	configs[2] = { {0.0f, 150.0f, 0.0f}, {0.0f, 80.0f, 0.0f}, 4.0f };	// 第3攻撃
	configs[3] = { {0.0f, 100.0f, 0.0f}, {0.0f, 50.0f, 0.0f}, 4.0f };	// 第4攻撃
	configs[4] = { {0.0f, 150.0f, 0.0f}, {0.0f, 80.0f, 0.0f}, 15.0f };	// 第5攻撃
}