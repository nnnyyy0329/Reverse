// 担当 : 成田

#pragma once
#include "PlayerBase.h"

class InteriorPlayer : public PlayerBase
{
public:
	InteriorPlayer();
	virtual ~InteriorPlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	// 共通関数のオーバーライド
	virtual void DebugRender()override;																		// デバッグ描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										// 弾による被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;					// 設定を取得
	virtual PlayerAnimations GetPlayerAnimation() override;				// アニメーション設定を取得
	virtual RenderConfig GetRenderConfig() override;					// 描画設定を取得
	virtual DodgeConfig GetDodgeConfig() override;						// 回避設定を取得
	virtual ShieldConfig GetShieldConfig() override;					// シールド設定を取得
	virtual AttackConstants GetAttackConstants()const override;			// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[5]) override;	// 攻撃設定を取得
	virtual AreaAttackConfig GetAreaAttackConfig() override;			// 範囲攻撃設定を取得

	/*****ゲッターセッター*****/

protected:

};

