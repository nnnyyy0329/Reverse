// 担当 : 成田

#include "InteriorPlayer.h"

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

InteriorPlayer::InteriorPlayer()
{
	// キャラタイプ
	SetCharaType(CHARA_TYPE::INTERIOR_PLAYER);
}

InteriorPlayer::~InteriorPlayer()
{

}

bool InteriorPlayer::Initialize()
{
	// 基底クラスの初期化
	PlayerBase::Initialize();

	return true;
}

bool InteriorPlayer::Terminate()
{
	return true;
}

bool InteriorPlayer::Process()
{
	// 基底クラスの更新処理
	PlayerBase::Process();

	return true;
}

bool InteriorPlayer::Render()
{
	PlayerBase::Render();

	// プレイヤーが死亡しているなら
	//if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	return true;
}

// デバッグ描画
void InteriorPlayer::DebugRender()
{
	// コリジョン描画
	PlayerBase::DebugRender();
}

// 被ダメージ処理
void InteriorPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 裏プレイヤーの情報設定
PlayerConfig InteriorPlayer::GetPlayerConfig()
{
	// 裏プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 3.0f;				
	config.normalMoveSpeed = 7.5f;				
	config.dashMoveSpeed = 3.0f;			

	// 基礎ステータス
	config.life = 100.0f;						
	config.maxLife = 100.0f;					
	config.startPos = VGet(0.0f, 0.0f, 0.0f);	

	// 表示設定
	config.drawSizeOffset = 16;					
	config.drawOffsetX = 900;					
	config.drawOffsetY = 0;						

	// モデル名
	config.modelName = "InteriorPlayer";			

	return config;
}

// 裏プレイヤーのアニメーション設定
PlayerAnimation InteriorPlayer::GetPlayerAnimation()
{
	// 裏プレイヤー用のアニメーション設定
	PlayerAnimation animation;

	animation.wait			= "player_idle_01";
	animation.walk			= "player_walk_01";
	animation.run			= "player_jog_01";
	animation.jumpUp		= "jump_up";
	animation.jumpDown		= "jump_down";
	animation.crouchWait	= "crouch_idle";
	animation.crouchWalk	= "crouch";
	animation.firstAttack	= "Nchange_attack_00";
	animation.secondAttack	= "Nchange_attack_01";
	animation.thirdAttack	= "Nchange_attack_02";
	animation.fourthAttack	= "EmotionNchange_attack_03";
	animation.fifthAttack	= "Nchange_attack_04";
	animation.hit			= "player_damage_00";
	animation.dodge			= "dodge";
	animation.death			= "player_dead_00";

	return animation;
}

// 表示設定
RenderConfig InteriorPlayer::GetRenderConfig()
{
	// 裏プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Interior Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

// 攻撃判定のパラメーター
AttackConstants InteriorPlayer::GetAttackConstants()
{
	// InteriorPlayer専用の攻撃定数
	AttackConstants constants;

	constants.attackOffsetScale = 80.0f;	// 攻撃判定オフセット倍率	
	constants.interiorMaxComboCount = 5;	// 裏プレイヤー用コンボカウント

	return constants;
}

// 攻撃判定の情報設定
void InteriorPlayer::GetAttackConfigs(AttackConfig configs[5])
{
	// 第1攻撃
	configs[0] = 
	{
		{0.0f, 80.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 120.0f, 0.0f}, 	// コリジョン下部位置
		25.0f,					// 半径
		8.0f,					// 発生フレーム
		12.0f,					// 持続フレーム
		18.0f,					// 硬直フレーム
		100.0f,					// ダメージ
		"",						// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット

	};

	// 第2攻撃
	configs[1] = 
	{
		{0.0f, 20.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 100.0f, 0.0f},	// コリジョン下部位置
		25.0f,					// 半径
		8.0f,					// 発生フレーム
		12.0f,					// 持続フレーム
		18.0f,					// 硬直フレーム
		100.0f,					// ダメージ
		"",						// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};

	// 第3攻撃
	configs[2] = 
	{ 
		{0.0f, 150.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 80.0f, 0.0f},	// コリジョン下部位置
		25.0f,					// 半径
		8.0f,					// 発生フレーム
		12.0f,					// 持続フレーム
		18.0f,					// 硬直フレーム
		100.0f,					// ダメージ
		"",						// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};

	// 第4攻撃
	configs[3] = 
	{
		{0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 50.0f, 0.0f},	// コリジョン下部位置
		25.0f,					// 半径
		8.0f,					// 発生フレーム
		12.0f,					// 持続フレーム
		18.0f,					// 硬直フレーム
		100.0f,					// ダメージ
		"",						// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};

	// 第5攻撃
	configs[4] = 
	{
		{0.0f, 150.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 80.0f, 0.0f},	// コリジョン下部位置
		25.0f,					// 半径
		8.0f,					// 発生フレーム
		12.0f,					// 持続フレーム
		18.0f,					// 硬直フレーム
		100.0f,					// ダメージ
		"",						// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
	};
}

// 回避設定データ構造体
DodgeConfig InteriorPlayer::GetDodgeConfig()
{
	// 裏プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::INTERIOR_PLAYER;
	config.invincibleDuration = 25.0f;  // 無敵時間（標準的）
	config.startTime = 4.0f;            // 開始時間（少し早め）
	config.activeTime = 18.0f;          // アクティブ時間
	config.recoveryTime = 12.0f;        // 硬直時間
	config.dodgeMoveSpeed = 9.0f;       // 移動速度（やや早め）

	return config;
}