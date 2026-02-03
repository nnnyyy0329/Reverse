// 担当 : 成田

#include "SurfacePlayer.h"

namespace
{
	// 基礎ステータス定数
	const float GRAVITY = -0.6f;		// 重力加速度
	const float DEFAULT_LIFE = 100.0f;	// デフォルト体力
	const float MAX_LIFE = 100.0f;		// 最大体力

	// 表示用定数
	const int DRAW_SIZE_OFFSET = 16;	// 描画サイズオフセット
	const int DRAW_OFFSET_X = 900;		// 描画Xオフセット
	const int DRAW_OFFSET_Y = 0;		// 描画Yオフセット
}

SurfacePlayer::SurfacePlayer()
{
	// キャラタイプ
	SetCharaType(CHARA_TYPE::SURFACE_PLAYER);
}

SurfacePlayer::~SurfacePlayer()
{

}

bool SurfacePlayer::Initialize()
{
	// 基底クラスの初期化
	PlayerBase::Initialize();

	return true;
}

bool SurfacePlayer::Terminate()
{
	return true;
}

bool SurfacePlayer::Process()
{
	// 基底クラスの更新処理
	PlayerBase::Process();

	return true;
}

bool SurfacePlayer::Render()
{
	// 基底クラスの描画処理呼び出し
	PlayerBase::Render();

	return true;
}

// デバッグ描画
void SurfacePlayer::DebugRender()
{
	// コリジョン描画
	PlayerBase::DebugRender();
}

// 被ダメージ処理
void SurfacePlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 表プレイヤーの情報設定
PlayerConfig SurfacePlayer::GetPlayerConfig()
{
	// 表プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 2.0f;				
	config.normalMoveSpeed = 5.0f;				
	config.dashMoveSpeed = 2.5f;			

	// 基礎ステータス
	config.life = 100.0f;						
	config.maxLife = 100.0f;					
	config.startPos = VGet(230.0f, 0.0f, 0.0f);	

	// 表示設定
	config.drawSizeOffset = 16;					
	config.drawOffsetX = 900;					
	config.drawOffsetY = 0;						

	// モデル名
	config.modelName = "SurfacePlayer";			

	return config;
}

// 表プレイヤーのアニメーション設定
PlayerAnimation SurfacePlayer::GetPlayerAnimation()
{
	// 表プレイヤー用のアニメーション設定
	PlayerAnimation animation;

	animation.wait			= "player_idle_00";
	animation.walk			= "player_walk_00";
	animation.run			= "player_jog_00";
	animation.jumpUp		= "jump_up";
	animation.jumpDown		= "jump_down";
	animation.crouchWait	= "crouch_idle";
	animation.crouchWalk	= "crouch";
	animation.firstAttack	= "absorb_attack_00";
	animation.secondAttack	= "absorb_attack_01";
	animation.thirdAttack	= "absorb_attack_02";
	animation.fourthAttack	= "";
	animation.fifthAttack	= "";
	animation.hit			= "player_damage_00";
	animation.dodge			= "dodge";
	animation.death			= "player_dead_00";

	return animation;
}

// 表示設定
RenderConfig SurfacePlayer::GetRenderConfig()
{
	// 表プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Surface Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

// 攻撃判定のパラメーター
AttackConstants SurfacePlayer::GetAttackConstants()
{
	// SurfacePlayer専用の攻撃定数
	AttackConstants constants;

	constants.ATTACK_OFFSET_SCALE = 85.0f;	// 攻撃判定オフセット倍率	
	constants.SURFACE_MAX_COMBO_COUNT = 3;	// 表プレイヤー用コンボカウント

	return constants;
}

// 攻撃判定の情報設定
void SurfacePlayer::GetAttackConfigs(AttackConfig configs[3])
{
	// 第1攻撃
	configs[0] = 
	{ 
		{0.0f, 90.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 50.0f, 0.0f},	// コリジョン下部位置
		30.0f,					// 半径
		20.0f,					// 発生フレーム
		25.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		25.0f,					// ダメージ
		"SurfacePlayerAttack1",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};	

	// 第2攻撃
	configs[1] = 
	{ 
		{0.0f, 80.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 10.0f, 0.0f},  	// コリジョン下部位置
		40.0f,					// 半径
		20.0f,					// 発生フレーム
		15.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		25.0f,					// ダメージ
		"SurfacePlayerAttack2",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};

	// 第3攻撃
	configs[2] =
	{ 
		{0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 0.0f, 0.0f},		// コリジョン下部位置
		25.0f,					// 半径
		20.0f,					// 発生フレーム
		25.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		50.0f,					// ダメージ
		"SurfacePlayerAttack3",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};
}