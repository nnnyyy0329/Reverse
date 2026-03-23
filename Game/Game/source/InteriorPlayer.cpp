// 担当 : 成田

#include "InteriorPlayer.h"

// 裏プレイヤー用定数のエイリアス
namespace IPC = InteriorPlayerConstants;

// 攻撃定数のエイリアス
namespace IAC = InteriorAttackConstants;

// 裏プレイヤーのフレームインデックス定数のエイリアス
namespace IPFIC = InteriorPlayerFrameIndexConstants;

InteriorPlayer::InteriorPlayer()
{
	// キャラタイプ
	SetCharaType(CHARA_TYPE::INTERIOR_PLAYER);
}

InteriorPlayer::~InteriorPlayer()
{

}

//bool InteriorPlayer::Initialize()
//{
//	// 基底クラスの初期化
//	PlayerBase::Initialize();
//
//	return true;
//}




bool InteriorPlayer::Initialize()
{
	// 基底クラスの初期化
	PlayerBase::Initialize();

	// 複数の攻撃タイプを初期化
	InitializeAttackTypeData(PLAYER_ATTACK_TYPE::NORMAL);
	InitializeAttackTypeData(PLAYER_ATTACK_TYPE::ABILITY);

	// デフォルトは通常攻撃
	SetCurrentAttackType(PLAYER_ATTACK_TYPE::NORMAL);

	return true;
}

// 攻撃タイプ別の最大コンボ数
int InteriorPlayer::GetMaxComboCountByType(PLAYER_ATTACK_TYPE type) const
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
			return IAC::ATTACK_COMBO_COUNT;	// 通常は5段コンボ

		case PLAYER_ATTACK_TYPE::ABILITY:
			return IAC::ABILITY_COMBO_COUNT;	// 重撃は3段コンボ

		default:
			return 0;
	}
}

// 攻撃タイプ別の設定を取得
void InteriorPlayer::GetAttackColConfigs(PLAYER_ATTACK_TYPE type, AttackCollision configs[])
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
		{
			GetNormalAttackConfigs(configs);
			break;
		}

		case PLAYER_ATTACK_TYPE::ABILITY :
		{
			GetAbilityAttackConfigs(configs);
			break;
		}
	}
}

// 通常攻撃設定（既存のコードをここに移動）
void InteriorPlayer::GetNormalAttackConfigs(AttackCollision configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].attackColTop = {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[0].attackColBottom = {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[0].attackColR = 40.0f,				// 半径
		configs[0].attackColTop = _vMove,				// 攻撃方向
		configs[0].attackDelay = 8.0f,					// 発生フレーム
		configs[0].attackDuration = 12.0f,				// 持続フレーム
		configs[0].attackRecovery = 18.0f,				// 硬直フレーム
		configs[0].damage = 20.0f,				// ダメージ
		configs[0].attackState = ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[0].attackMoveSpeed = 3.0f,					// 攻撃中の移動速度
		configs[0].isActive = false,				// アクティブかどうか
		configs[0].isHit = false,				// ヒットしたかどうか
		configs[0].canKnockback = false,				// 吹き飛ばし攻撃かどうか
		configs[0].isAttackCancelByHit = true,					// 被弾でキャンセルされる攻撃か
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].attackColTop = {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[1].attackColBottom = {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[1].attackColR = 40.0f,				// 半径
		configs[1].attackColTop = _vMove,				// 攻撃方向
		configs[1].attackDelay = 8.0f,					// 発生フレーム
		configs[1].attackDuration = 12.0f,				// 持続フレーム
		configs[1].attackRecovery = 18.0f,				// 硬直フレーム
		configs[1].damage = 30.0f,				// ダメージ
		configs[1].attackState = ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[1].attackMoveSpeed = 0.0f,					// 攻撃中の移動速度
		configs[1].isActive = false,				// アクティブかどうか
		configs[1].isHit = false,				// ヒットしたかどうか
		configs[1].canKnockback = false,				// 吹き飛ばし攻撃かどうか
		configs[1].isAttackCancelByHit = true,					// 被弾でキャンセルされる攻撃か
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].attackColTop = {0.0f, 150.0f, 0.0f},	// コリジョン上部位置
		configs[2].attackColBottom = {0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		configs[2].attackColR = 40.0f,				// 半径
		configs[2].attackColTop = _vMove,				// 攻撃方向
		configs[2].attackDelay = 10.0f,				// 発生フレーム
		configs[2].attackDuration = 12.0f,				// 持続フレーム
		configs[2].attackRecovery = 18.0f,				// 硬直フレーム
		configs[2].damage = 50.0f,				// ダメージ
		configs[2].attackState = ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[2].attackMoveSpeed = 5.0f,					// 攻撃中の移動速度
		configs[2].isActive = false,				// アクティブかどうか
		configs[2].isHit = false,				// ヒットしたかどうか
		configs[2].canKnockback = false,				// 吹き飛ばし攻撃かどうか
		configs[2].isAttackCancelByHit = true,					// 被弾でキャンセルされる攻撃か
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].attackColTop = {0.0f, 70.0f, 0.0f},	// コリジョン上部位置
		configs[3].attackColBottom = {0.0f, 0.0f, 0.0f},	// コリジョン下部位置
		configs[3].attackColR = 40.0f,				// 半径
		configs[3].attackColTop = _vMove,				// 攻撃方向
		configs[3].attackDelay = 15.0f,				// 発生フレーム
		configs[3].attackDuration = 12.0f,				// 持続フレーム
		configs[3].attackRecovery = 23.0f,				// 硬直フレーム
		configs[3].damage = 50.0f,				// ダメージ
		configs[3].attackState = ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[3].attackMoveSpeed = 5.0f,					// 攻撃中の移動速度
		configs[3].isActive = false,				// アクティブかどうか
		configs[3].isHit = false,				// ヒットしたかどうか
		configs[3].canKnockback = false,				// 吹き飛ばし攻撃かどうか
		configs[3].isAttackCancelByHit = true,					// 被弾でキャンセルされる攻撃か
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].attackColTop = {0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		configs[4].attackColBottom = {0.0f, 10.0f, 0.0f},	// コリジョン下部位置
		configs[4].attackColR = 80.0f,				// 半径
		configs[4].attackColTop = _vMove,				// 攻撃方向
		configs[4].attackDelay = 30.0f,				// 発生フレーム
		configs[4].attackDuration = 30.0f,				// 持続フレーム
		configs[4].attackRecovery = 0.0f,					// 硬直フレーム
		configs[4].damage = 100.0f,				// ダメージ
		configs[4].attackState = ATTACK_STATE::ACTIVE,	// 攻撃状態
		configs[4].attackMoveSpeed = 15.0f,				// 攻撃中の移動速度
		configs[4].isActive = false,				// アクティブかどうか
		configs[4].isHit = false,				// ヒットしたかどうか
		configs[4].canKnockback = true,					// 吹き飛ばし攻撃かどうか
		configs[4].isAttackCancelByHit = false,				// 被弾でキャンセルされる攻撃か
	};
}

// 能力攻撃設定（新規作成）
void InteriorPlayer::GetAbilityAttackConfigs(AttackCollision configs[])
{
	// 重撃は強力だが段階が少ない
	configs[0] =
	{
		{0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 20.0f, 0.0f},	// コリジョン下部位置
		100.0f,					// 半径（通常より大きい）
		_vMove,					// 攻撃方向
		30.0f,					// 発生フレーム
		30.0f,					// 持続フレーム
		0.0f,					// 硬直フレーム
		60.0f,					// ダメージ（通常より大きい）
		ATTACK_STATE::ACTIVE,	// 攻撃状態
		20.0f,					// 攻撃中の移動速度
		true,					// 吹き飛ばし攻撃
		true					// 被弾でキャンセル
	};
}

// 攻撃タイプ別のエフェクト設定を取得
void InteriorPlayer::GetAttackEffectConfigs(PLAYER_ATTACK_TYPE type, AttackEffectConfig configs[])
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
		{
			GetNormalAttackEffectConfigs(configs);
			break;
		}

		case PLAYER_ATTACK_TYPE::ABILITY:
		{
			GetAbilityAttackEffectConfigs(configs);
			break;
		}
	}
}

void InteriorPlayer::GetNormalAttackEffectConfigs(AttackEffectConfig configs[])
{
	// 第1攻撃
	configs[0] =
	{
		/* エフェクト */
		configs[0].isActiveEffect = true,							// エフェクトを有効にするか
		configs[0].effectName = "InteriorPlayerAttack_123",	// エフェクト名
		configs[0].effectOffset = { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		configs[0].effectRotation = { 0.0f, 180.0f, 0.0f },		// エフェクト回転オフセット
		configs[0].attachType = EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

		/* サウンド */
		configs[0].isActiveSound = true,				// サウンドを有効にするか
		configs[0].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[0].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[0].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		configs[0].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[0].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[0].hitStopDuration = 0.0f,		// ヒットストップの持続時間
	};

	// 第2攻撃
	configs[1] =
	{
		/* エフェクト */
		configs[1].isActiveEffect = true,								// エフェクトを有効にするか
		configs[1].effectName = "InteriorPlayerAttack_123",		// エフェクト名
		configs[1].effectOffset = { 0.0f, 0.0f, 0.0f },				// エフェクト位置オフセット
		configs[1].effectRotation = { 0.0f, 90.0f, 0.0f },			// エフェクト回転オフセット
		configs[1].attachType = EFFECT_ATTACH_TYPE::RIGHT_ARM,	// エフェクトの追従方法

		/* サウンド */
		configs[1].isActiveSound = true,				// サウンドを有効にするか
		configs[1].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[1].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[1].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		configs[1].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[1].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[1].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第3攻撃
	configs[2] =
	{
		/* エフェクト */
		configs[2].isActiveEffect = true,							// エフェクトを有効にするか
		configs[2].effectName = "InteriorPlayerAttack_123",	// エフェクト名
		configs[2].effectOffset = { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		configs[2].effectRotation = { 90.0f, 0.0f, 0.0f },		// エフェクト回転オフセット
		configs[2].attachType = EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

		/* サウンド */
		configs[2].isActiveSound = true,				// サウンドを有効にするか
		configs[2].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[2].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[2].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		configs[2].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[2].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[2].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第4攻撃
	configs[3] =
	{
		/* エフェクト */
		configs[3].isActiveEffect = true,									// エフェクトを有効にするか
		configs[3].effectName = "InteriorPlayerFourthAttack",			// エフェクト名
		configs[3].effectOffset = { 0.0f, 150.0f, 0.0f },				// エフェクト位置オフセット
		configs[3].effectRotation = { 0.0f, -90.0f, 0.0f },				// エフェクト回転オフセット
		configs[3].attachType = EFFECT_ATTACH_TYPE::CHARACTER_OFFSET,	// エフェクトの追従方法

		/* サウンド */
		configs[3].isActiveSound = true,				// サウンドを有効にするか
		configs[3].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[3].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[3].cameraShakeMagnitude = 5.0f,	// カメラ振動の強さ
		configs[3].cameraShakeDuration = 5.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[3].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[3].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};

	// 第5攻撃
	configs[4] =
	{
		/* エフェクト */
		configs[4].isActiveEffect = true,							// エフェクトを有効にするか
		configs[4].effectName = "InteriorPlayerFifthAttack",	// エフェクト名
		configs[4].effectOffset = { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		configs[4].effectRotation = { 0.0f, 180.0f, 0.0f },		// エフェクト回転オフセット
		configs[4].attachType = EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

		/* サウンド */
		configs[4].isActiveSound = true,				// サウンドを有効にするか
		configs[4].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[4].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[4].cameraShakeMagnitude = 15.0f,	// カメラ振動の強さ
		configs[4].cameraShakeDuration = 10.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[4].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[4].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};
}

void InteriorPlayer::GetAbilityAttackEffectConfigs(AttackEffectConfig configs[])
{
	// 第1攻撃
	configs[0] =
	{
		/* エフェクト */
		configs[0].isActiveEffect = true,							// エフェクトを有効にするか
		configs[0].effectName = "InteriorPlayerFifthAttack",	// エフェクト名
		configs[0].effectOffset = { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		configs[0].effectRotation = { 0.0f, 180.0f, 0.0f },		// エフェクト回転オフセット
		configs[0].attachType = EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

		/* サウンド */
		configs[0].isActiveSound = true,				// サウンドを有効にするか
		configs[0].soundName = "iPlayerAttack",	// サウンド名

		/* カメラシェイク */
		configs[0].isActiveCameraShake = true,	// カメラシェイクを有効にするか
		configs[0].cameraShakeMagnitude = 15.0f,	// カメラ振動の強さ
		configs[0].cameraShakeDuration = 10.0f,	// カメラ振動の持続時間

		/* ヒットストップ */
		configs[0].isActiveHitStop = false,	// ヒットストップを有効にするか
		configs[0].hitStopDuration = 0.0f,	// ヒットストップの持続時間
	};
}

void InteriorPlayer::GetAttackColOffsetConfigs(PLAYER_ATTACK_TYPE type, AttackColOffset configs[])
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
		{
			GetNormalAttackColOffsetConfigs(configs);
			break;
		}

		case PLAYER_ATTACK_TYPE::ABILITY:
		{
			GetAbilityAttackColOffsetConfigs(configs);
			break;
		}

		default:
			break;
	}
}

void InteriorPlayer::GetNormalAttackColOffsetConfigs(AttackColOffset configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].directionScale = 100.0f,	// 方向スケール
		configs[0].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].directionScale = 100.0f,	// 方向スケール
		configs[1].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].directionScale = 100.0f,	// 方向スケール
		configs[2].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].directionScale = 100.0f,	// 方向スケール
		configs[3].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].directionScale = 75.0f,	// 方向スケール
		configs[4].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};
}

void InteriorPlayer::GetAbilityAttackColOffsetConfigs(AttackColOffset configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].directionScale = 150.0f,	// 方向スケール（通常より大きい）
		configs[0].useOwnerDirection = true,		// 所有者の向きを基準とするか
	};
}

void InteriorPlayer::GetAttackDirAdjustConfigs(PLAYER_ATTACK_TYPE type, AttackDirAdjustConfig configs[])
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
		{
			GetNormalAttackDirAdjustConfigs(configs);
			break;
		}
		case PLAYER_ATTACK_TYPE::ABILITY:
		{
			GetAbilityAttackDirAdjustConfigs(configs);
			break;
		}
		default:
			break;
	}
}

void InteriorPlayer::GetNormalAttackDirAdjustConfigs(AttackDirAdjustConfig configs[])
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

void InteriorPlayer::GetAbilityAttackDirAdjustConfigs(AttackDirAdjustConfig configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].canDirAdjust = true,	// 向き調整が可能かどうか
	};
}

void InteriorPlayer::GetAttackArmConfigs(PLAYER_ATTACK_TYPE type, AttackArmConfig configs[])
{
	switch(type)
	{
		case PLAYER_ATTACK_TYPE::NORMAL:
		{
			GetNormalAttackArmConfigs(configs);
			break;
		}
		case PLAYER_ATTACK_TYPE::ABILITY:
		{
			GetAbilityAttackArmConfigs(configs);
			break;
		}

		default:
			break;
	}
}

void InteriorPlayer::GetNormalAttackArmConfigs(AttackArmConfig configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[0].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[1].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[2].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[3].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[4].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};
}

void InteriorPlayer::GetAbilityAttackArmConfigs(AttackArmConfig configs[])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[0].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};
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

void InteriorPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE ownerType, const AttackCollision& attackInfo)
{
	// ダメージに裏プレイヤー専用の倍率を適用
	float resultDamage = fDamage * IPC::DAMAGE_MULTIPLIER;

	// 基底クラスの被ダメージ処理を呼び出す
	PlayerBase::ApplyDamage(resultDamage, ownerType, attackInfo);

	// 特定の所有者の攻撃移動を停止
	AttackManager::GetInstance()->StopAttackMovementByOwner(ATTACK_OWNER_TYPE::INTERIOR_PLAYER);
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

	constants.interiorMaxComboCount = IAC::ATTACK_COMBO_COUNT;	// 裏プレイヤー用コンボカウント

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
		config[0].isActiveEffect	= true,							// エフェクトを有効にするか
		config[0].effectName		= "InteriorPlayerAttack_123",	// エフェクト名
		config[0].effectOffset		= { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		config[0].effectRotation	= { 0.0f, 180.0f, 0.0f },		// エフェクト回転オフセット
		config[0].attachType		= EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

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
		config[1].isActiveEffect	= true,								// エフェクトを有効にするか
		config[1].effectName		= "InteriorPlayerAttack_123",		// エフェクト名
		config[1].effectOffset		= { 0.0f, 0.0f, 0.0f },				// エフェクト位置オフセット
		config[1].effectRotation	= { 0.0f, 90.0f, 0.0f },			// エフェクト回転オフセット
		config[1].attachType		= EFFECT_ATTACH_TYPE::RIGHT_ARM,	// エフェクトの追従方法

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
		config[2].isActiveEffect	= true,							// エフェクトを有効にするか
		config[2].effectName		= "InteriorPlayerAttack_123",	// エフェクト名
		config[2].effectOffset		= { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		config[2].effectRotation	= { 90.0f, 0.0f, 0.0f },		// エフェクト回転オフセット
		config[2].attachType		= EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

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
		config[3].isActiveEffect	= true,									// エフェクトを有効にするか
		config[3].effectName		= "InteriorPlayerFourthAttack",			// エフェクト名
		config[3].effectOffset		= { 0.0f, 150.0f, 0.0f },				// エフェクト位置オフセット
		config[3].effectRotation	= { 0.0f, -90.0f, 0.0f },				// エフェクト回転オフセット
		config[3].attachType		= EFFECT_ATTACH_TYPE::CHARACTER_OFFSET,	// エフェクトの追従方法

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
		config[4].isActiveEffect	= true,							// エフェクトを有効にするか
		config[4].effectName		= "InteriorPlayerFifthAttack",	// エフェクト名
		config[4].effectOffset		= { 0.0f, 0.0f, 0.0f },			// エフェクト位置オフセット
		config[4].effectRotation	= { 0.0f, 180.0f, 0.0f },		// エフェクト回転オフセット
		config[4].attachType		= EFFECT_ATTACH_TYPE::LEFT_ARM,	// エフェクトの追従方法

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

//AreaAttackConfig InteriorPlayer::GetAreaAttackConfigs()
//{
//	// 裏プレイヤーの範囲攻撃設定
//	AreaAttackConfig config;
//
//	config.centerOffset	= VGet(0.0f, 0.0f, 0.0f);	// コリジョン中心位置オフセット
//	config.radius		= 50.0f;					// 半径
//	config.height		= 10.0f;					// 高さ
//	config.delay		= 10.0f;					// 発生フレーム
//	config.duration		= 20.0f;					// 持続フレーム
//	config.recovery		= 30.0f;					// 硬直フレーム
//	config.damage		= 40.0f;					// ダメージ
//	config.isHit		= false;					// ヒットフラグ
//
//	return config;
//}

DodgeConfig InteriorPlayer::GetDodgeConfig()
{
	// 裏プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::INTERIOR_PLAYER;
	config.invincibleDuration	= 25.0f;		// 無敵時間
	config.startTime			= 1.0f;			// 開始時間
	config.activeTime			= 30.0f;		// アクティブ時間
	config.recoveryTime			= 10.0f;		// 硬直時間
	config.dodgeMoveSpeed		= 11.0f;		// 移動速度
	config.soundName = "SE_TransPlayerDodge";	// サウンド名

	return config;
}

void InteriorPlayer::GetAttackArmConfigs(AttackArmConfig configs[5])
{
	// 第1攻撃
	configs[0] =
	{
		configs[0].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[0].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第2攻撃
	configs[1] =
	{
		configs[1].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[1].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第3攻撃
	configs[2] =
	{
		configs[2].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[2].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第4攻撃
	configs[3] =
	{
		configs[3].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[3].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};

	// 第5攻撃
	configs[4] =
	{
		configs[4].rightArmFrameIndex = IPFIC::RIGHT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
		configs[4].leftArmFrameIndex = IPFIC::LEFT_ARM_FRAME_INDEX,	// 攻撃腕位置オフセット
	};
}