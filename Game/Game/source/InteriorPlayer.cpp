// 担当 : 成田

#include "InteriorPlayer.h"

// 定数のエイリアス
namespace IA = InteriorAttackConstants;

// 腕の定数のエイリアス
namespace IPA = InteriorPlayerArmConstants;

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

void InteriorPlayer::DebugRender()
{
	// コリジョン描画
	PlayerBase::DebugRender();
}

void InteriorPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo)
{
	// 基底クラスの被ダメージ処理を呼び出す
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

void InteriorPlayer::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)
{
	// 基底クラスの弾による被ダメージ処理を呼び出す
	PlayerBase::ApplyDamageByBullet(fDamage, chara);
}

PlayerConfig InteriorPlayer::GetPlayerConfig()
{
	// 裏プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 3.0f;				
	config.normalMoveSpeed = 7.5f;				
	config.dashMoveSpeed = 2.6f;			

	// 基礎ステータス
	config.life = 200.0f;
	config.maxLife = 200.0f;

	// 表示設定
	config.drawSizeOffset = 16;					
	config.drawOffsetX = 900;					
	config.drawOffsetY = 0;						

	// モデル名
	config.modelName = "InteriorPlayer";			

	return config;
}

PlayerAnimations InteriorPlayer::GetPlayerAnimation()
{
	// 裏プレイヤー用のアニメーション設定
	PlayerAnimations animation;

	animation.movement.wait			= "player_idle_01";
	animation.movement.walk			= "player_walk_01";
	animation.movement.run			= "player_jog_01";
	animation.movement.jumpUp		= "";
	animation.movement.jumpDown		= "";
	animation.movement.crouchWait	= "";
	animation.movement.crouchWalk	= "";
	animation.attack.firstAttack	= "Nchange_attack_00";
	animation.attack.secondAttack	= "Nchange_attack_01";
	animation.attack.thirdAttack	= "Nchange_attack_02";
	animation.attack.fourthAttack	= "Nchange_attack_03";
	animation.attack.fifthAttack	= "Nchange_attack_04";
	animation.attack.firstSkill		= "";
	animation.attack.secondSkill	= "";
	animation.absorb.absorbReady	= "";
	animation.absorb.absorbActive	= "";
	animation.absorb.absorbEnd		= "";
	animation.combat.transform		= "";
	animation.combat.transCancel	= "player_cancell_00";
	animation.combat.guard			= "";
	animation.combat.hit			= "player_damage_01";
	animation.combat.dodge			= "player_dodge_01";
	animation.combat.death			= "player_dead_01";

	return animation;
}

RenderConfig InteriorPlayer::GetRenderConfig()
{
	// 裏プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Interior Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

AttackConstants InteriorPlayer::GetAttackConstants()const
{
	// InteriorPlayer専用の攻撃定数
	AttackConstants constants;

	constants.interiorMaxComboCount = InteriorAttackConstants::INTERIOR_MAX_COMBO_COUNT;	// 裏プレイヤー用コンボカウント

	return constants;
}

void InteriorPlayer::GetAttackColConfigs(AttackCollision configs[5])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].attackColTop			= {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[0].attackColBottom		= {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[0].attackColR			= 40.0f,				// 半径
		configs[0].attackColTop			= _vMove,				// 攻撃方向
		configs[0].attackDelay			= 8.0f,					// 発生フレーム
		configs[0].attackDuration		= 12.0f,				// 持続フレーム
		configs[0].attackRecovery		= 18.0f,				// 硬直フレーム
		configs[0].damage				= 20.0f,				// ダメージ
		configs[0].attackState			= ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[0].attackMoveSpeed		= 3.0f,					// 攻撃中の移動速度
		configs[0].isActive				= false,				// アクティブかどうか
		configs[0].isHit				= false,				// ヒットしたかどうか
		configs[0].canKnockback			= false,				// 吹き飛ばし攻撃かどうか
		configs[0].isAttackCancelByHit	= true,					// 被弾でキャンセルされる攻撃か
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].attackColTop			= {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[1].attackColBottom		= {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[1].attackColR			= 40.0f,				// 半径
		configs[1].attackColTop			= _vMove,				// 攻撃方向
		configs[1].attackDelay			= 8.0f,					// 発生フレーム
		configs[1].attackDuration		= 12.0f,				// 持続フレーム
		configs[1].attackRecovery		= 18.0f,				// 硬直フレーム
		configs[1].damage				= 30.0f,				// ダメージ
		configs[1].attackState			= ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[1].attackMoveSpeed		= 0.0f,					// 攻撃中の移動速度
		configs[1].isActive				= false,				// アクティブかどうか
		configs[1].isHit				= false,				// ヒットしたかどうか
		configs[1].canKnockback			= false,				// 吹き飛ばし攻撃かどうか
		configs[1].isAttackCancelByHit	= true,					// 被弾でキャンセルされる攻撃か
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].attackColTop			= {0.0f, 150.0f, 0.0f},	// コリジョン上部位置
		configs[2].attackColBottom		= {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[2].attackColR			= 40.0f,				// 半径
		configs[2].attackColTop			= _vMove,				// 攻撃方向
		configs[2].attackDelay			= 10.0f,				// 発生フレーム
		configs[2].attackDuration		= 12.0f,				// 持続フレーム
		configs[2].attackRecovery		= 18.0f,				// 硬直フレーム
		configs[2].damage				= 50.0f,				// ダメージ
		configs[2].attackState			= ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[2].attackMoveSpeed		= 5.0f,					// 攻撃中の移動速度
		configs[2].isActive				= false,				// アクティブかどうか
		configs[2].isHit				= false,				// ヒットしたかどうか
		configs[2].canKnockback			= false,				// 吹き飛ばし攻撃かどうか
		configs[2].isAttackCancelByHit	= true,					// 被弾でキャンセルされる攻撃か
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].attackColTop			= {0.0f, 70.0f, 0.0f},	// コリジョン上部位置
		configs[3].attackColBottom		= {0.0f, 0.0f, 0.0f},	// コリジョン下部位置
		configs[3].attackColR			= 40.0f,				// 半径
		configs[3].attackColTop			= _vMove,				// 攻撃方向
		configs[3].attackDelay			= 15.0f,				// 発生フレーム
		configs[3].attackDuration		= 12.0f,				// 持続フレーム
		configs[3].attackRecovery		= 23.0f,				// 硬直フレーム
		configs[3].damage				= 50.0f,				// ダメージ
		configs[3].attackState			= ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[3].attackMoveSpeed		= 5.0f,					// 攻撃中の移動速度
		configs[3].isActive				= false,				// アクティブかどうか
		configs[3].isHit				= false,				// ヒットしたかどうか
		configs[3].canKnockback			= false,				// 吹き飛ばし攻撃かどうか
		configs[3].isAttackCancelByHit	= true,					// 被弾でキャンセルされる攻撃か
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].attackColTop			= {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[4].attackColBottom		= {0.0f, 10.0f, 0.0f},	// コリジョン下部位置
		configs[4].attackColR			= 80.0f,				// 半径
		configs[4].attackColTop			= _vMove,				// 攻撃方向
		configs[4].attackDelay			= 30.0f,				// 発生フレーム
		configs[4].attackDuration		= 30.0f,				// 持続フレーム
		configs[4].attackRecovery		= 0.0f,					// 硬直フレーム
		configs[4].damage				= 100.0f,				// ダメージ
		configs[4].attackState			= ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[4].attackMoveSpeed		= 15.0f,				// 攻撃中の移動速度
		configs[4].isActive				= false,				// アクティブかどうか
		configs[4].isHit				= false,				// ヒットしたかどうか
		configs[4].canKnockback			= true,					// 吹き飛ばし攻撃かどうか
		configs[4].isAttackCancelByHit	= false,				// 被弾でキャンセルされる攻撃か
	};
}

void InteriorPlayer::GetAttackColOffsetConfigs(AttackColOffset configs[5])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].directionScale		= 100.0f,	// 方向スケール
		configs[0].useOwnerDirection	= true,		// 所有者の向きを基準とするか
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].directionScale		= 100.0f,	// 方向スケール
		configs[1].useOwnerDirection	= true,		// 所有者の向きを基準とするか
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].directionScale		= 100.0f,	// 方向スケール
		configs[2].useOwnerDirection	= true,		// 所有者の向きを基準とするか
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].directionScale		= 100.0f,	// 方向スケール
		configs[3].useOwnerDirection	= true,		// 所有者の向きを基準とするか
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].directionScale		= 75.0f,	// 方向スケール
		configs[4].useOwnerDirection	= true,		// 所有者の向きを基準とするか
	};
}

void InteriorPlayer::GetAttackDirAdjustConfigs(AttackDirAdjustConfig configs[5])
{
	// 第1攻撃
	configs[0] = 
	{
		configs[0].canDirAdjust = true,	// 向き調整が可能かどうか
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].canDirAdjust = true,	// 向き調整が可能かどうか
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].canDirAdjust = true,	// 向き調整が可能かどうか
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].canDirAdjust = true,	// 向き調整が可能かどうか
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].canDirAdjust = true,	// 向き調整が可能かどうか
	};
}

void InteriorPlayer::GetAttackEffectConfigs(AttackEffectConfig config[5])
{
	// 第1攻撃
	config[0] =
	{
		/* エフェクト */
		config[0].isActiveEffect	= true,										// エフェクトを有効にするか
		config[0].effectName		= "InteriorPlayerAttack_123",				// エフェクト名
		config[0].effectOffset		= { 0.0f, 0.0f, 0.0f },						// エフェクト位置オフセット
		config[0].effectRotation	= { 0.0f, IA::REVERSE_EFFECT_ROT, 0.0f },	// エフェクト回転オフセット

		/* サウンド */
		config[0].isActiveSound = true,				// サウンドを有効にするか
		config[0].soundName		= "iPlayerAttack",	// サウンド名
		
		/* カメラシェイク */
		config[0].isActiveCameraShake	= true,	// カメラシェイクを有効にするか
		config[0].cameraShakeMagnitude	= 5.0f,	// カメラ振動の強さ
		config[0].cameraShakeDuration	= 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		config[0].isActiveHitStop	= false,	// ヒットストップを有効にするか
		config[0].hitStopDuration	= 0.0f,		// ヒットストップの持続時間
	};

	// 第2攻撃
	config[1] =
	{
		/* エフェクト */
		config[1].isActiveEffect	= true,										// エフェクトを有効にするか
		config[1].effectName		= "InteriorPlayerAttack_123",				// エフェクト名
		config[1].effectOffset		= { 0.0f, 0.0f, 0.0f },						// エフェクト位置オフセット
		config[1].effectRotation	= { 0.0f, IA::REVERSE_EFFECT_ROT, 0.0f },	// エフェクト回転オフセット

		/* サウンド */
		config[1].isActiveSound = true,				// サウンドを有効にするか
		config[1].soundName		= "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		config[1].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		config[1].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		config[1].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		config[1].isActiveHitStop = false,	// ヒットストップを有効にするか
		config[1].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第3攻撃
	config[2] =
	{
		/* エフェクト */
		config[2].isActiveEffect	= true,										// エフェクトを有効にするか
		config[2].effectName		= "InteriorPlayerAttack_123",				// エフェクト名
		config[2].effectOffset		= { 0.0f, 0.0f, 0.0f },						// エフェクト位置オフセット
		config[2].effectRotation	= { 0.0f, IA::REVERSE_EFFECT_ROT, 0.0f },	// エフェクト回転オフセット

		/* サウンド */
		config[2].isActiveSound = true,				// サウンドを有効にするか
		config[2].soundName		= "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		config[2].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		config[2].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		config[2].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		config[2].isActiveHitStop = false,	// ヒットストップを有効にするか
		config[2].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第4攻撃
	config[3] =
	{
		/* エフェクト */
		config[3].isActiveEffect	= true,							// エフェクトを有効にするか
		config[3].effectName		= "InteriorPlayerFourthAttack",	// エフェクト名
		config[3].effectOffset		= { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		config[3].effectRotation	= { 0.0f, 90.0f, 0.0f },		// エフェクト回転オフセット

		/* サウンド */
		config[3].isActiveSound = true,				// サウンドを有効にするか
		config[3].soundName		= "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		config[3].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		config[3].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		config[3].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		config[3].isActiveHitStop = false,	// ヒットストップを有効にするか
		config[3].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第5攻撃
	config[4] =
	{
		/* エフェクト */
		config[4].isActiveEffect	= true,										// エフェクトを有効にするか
		config[4].effectName		= "InteriorPlayerFifthAttack",				// エフェクト名
		config[4].effectOffset		= { 0.0f, 0.0f, 0.0f },						// エフェクト位置オフセット
		config[4].effectRotation	= { 0.0f, IA::REVERSE_EFFECT_ROT, 0.0f },	// エフェクト回転オフセット

		/* サウンド */
		config[4].isActiveSound = true,				// サウンドを有効にするか
		config[4].soundName		= "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		config[4].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		config[4].cameraShakeMagnitude = 15.0f,	// カメラ振動の強さ
		config[4].cameraShakeDuration = 10.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		config[4].isActiveHitStop = false,	// ヒットストップを有効にするか
		config[4].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};
}

AreaAttackConfig InteriorPlayer::GetAreaAttackConfigs()
{
	// 裏プレイヤーの範囲攻撃設定
	AreaAttackConfig config;

	config.centerOffset	= VGet(0.0f, 0.0f, 0.0f);	// コリジョン中心位置オフセット
	config.radius		= 50.0f;					// 半径
	config.height		= 10.0f;					// 高さ
	config.delay		= 10.0f;					// 発生フレーム
	config.duration		= 20.0f;					// 持続フレーム
	config.recovery		= 30.0f;					// 硬直フレーム
	config.damage		= 40.0f;					// ダメージ
	config.isHit		= false;					// ヒットフラグ

	return config;
}

DodgeConfig InteriorPlayer::GetDodgeConfig()
{
	// 裏プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::INTERIOR_PLAYER;
	config.invincibleDuration	= 25.0f;	// 無敵時間
	config.startTime			= 1.0f;		// 開始時間
	config.activeTime			= 30.0f;	// アクティブ時間
	config.recoveryTime			= 10.0f;	// 硬直時間
	config.dodgeMoveSpeed		= 11.0f;	// 移動速度

	return config;
}

void InteriorPlayer::GetAttackArmConfigs(AttackArmConfig configs[5])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].useFromBody = 0,									// 左腕を使用
		configs[0].rightArmFrameIndex = IPA::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[0].leftArmFrameIndex = IPA::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].useFromBody = 1,									// 右腕を使用
		configs[1].rightArmFrameIndex = IPA::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[1].leftArmFrameIndex = IPA::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].useFromBody = 0,									// 左腕を使用
		configs[2].rightArmFrameIndex = IPA::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[2].leftArmFrameIndex = IPA::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].useFromBody = 2,									// 左腕を使用
		configs[3].rightArmFrameIndex = IPA::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[3].leftArmFrameIndex = IPA::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].useFromBody = 0,									// 左腕を使用
		configs[4].rightArmFrameIndex = IPA::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[4].leftArmFrameIndex = IPA::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};
}