// 担当 : 成田

#pragma once
#include "PlayerBase.h"

// 攻撃判定のパラメーター
namespace InteriorAttackConstants
{
	constexpr int INTERIOR_MAX_COMBO_COUNT = 5;		// 裏プレイヤー用コンボカウント

	constexpr float REVERSE_EFFECT_ROT = 90.0f;	// エフェクトの回転を反転
}

// 攻撃腕のパラメーター
namespace InteriorPlayerArmConstants
{
	constexpr int RIGHT_ARM_FRAME_INDEX = 157;	// 右腕攻撃のフレームインデックス
	constexpr int LEFT_ARM_FRAME_INDEX = 147;	// 左腕攻撃のフレームインデックス
}

/// @brief 裏プレイヤークラス
class InteriorPlayer : public PlayerBase
{
public:
	InteriorPlayer();
	virtual ~InteriorPlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	/// @brief デバッグ描画
	virtual void DebugRender()override;

	/// @brief 被ダメージ処理
	/// 
	/// @param fDamage ダメージ量
	/// @param eType 攻撃の所有者タイプ
	/// @param attackInfo 攻撃コリジョン情報
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo) override;

	/// @brief 弾による被ダメージ処理
	///
	/// @param fDamage ダメージ量
	/// @param chara 弾のキャラタイプ
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara, BULLET_TYPE bulletType)override;


	/* 純粋仮想関数のオーバーライド */
	
	/// @brief プレイヤー設定を取得
	///
	/// @return プレイヤー設定構造体
	virtual PlayerConfig		GetPlayerConfig()											override;

	/// @brief プレイヤーアニメーション設定を取得
	///
	/// @return プレイヤーアニメーション設定構造体
	virtual PlayerAnimations	GetPlayerAnimation()										override;

	/// @brief 描画設定を取得
	///
	/// @return 描画設定構造体
	virtual RenderConfig		GetRenderConfig()											override;	

	/// @brief 回避設定を取得
	///
	/// @return 回避設定構造体
	virtual DodgeConfig			GetDodgeConfig()											override;	

	/// @brief 攻撃定数を取得
	///
	/// @return 攻撃定数構造体
	virtual AttackConstants		GetAttackConstants()const									override;	

	/// @brief 攻撃コリジョン設定を取得
	///
	/// @param configs[5] 攻撃コリジョン設定の配列(5段階分)
	virtual void				GetAttackColConfigs(AttackCollision configs[5])				override;

	/// @brief 攻撃コリジョンオフセット設定を取得
	///
	/// @param configs[5] 攻撃コリジョンオフセット設定の配列(5段階分)
	virtual void				GetAttackColOffsetConfigs(AttackColOffset configs[5])		override;

	/// @brief 攻撃方向補正設定を取得
	///
	/// @param configs[5] 攻撃方向補正設定の配列(5段階分)
	virtual void				GetAttackDirAdjustConfigs(AttackDirAdjustConfig configs[5])	override;

	/// @brief 範囲攻撃設定を取得
	///
	/// @return 範囲攻撃設定構造体
	virtual AreaAttackConfig	GetAreaAttackConfigs()										override;

	/// @brief 演出設定を取得
	///
	/// @param configs[5] 演出設定の配列(5段階分)
	virtual void				GetAttackEffectConfigs(AttackEffectConfig configs[5])		override;

	/// @brief 攻撃腕設定を取得
	///
	/// @param configs[5] 攻撃腕設定の配列(5段階分)
	virtual void				GetAttackArmConfigs(AttackArmConfig configs[5])				override;	


	/*****ゲッターセッター*****/

protected:

};

