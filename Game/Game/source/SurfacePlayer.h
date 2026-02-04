// 担当 : 成田

#pragma once
#include "PlayerBase.h"

class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	
	virtual void DebugRender()override;									// デバッグ情報描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;		// 設定を取得
	virtual PlayerAnimation GetPlayerAnimation() override;	// アニメーション設定を取得
	virtual RenderConfig GetRenderConfig() override;		// 描画設定を取得
	virtual DodgeConfig GetDodgeConfig() override;			// 回避設定を取得

	/*****ゲッターセッター*****/

protected:
	// 攻撃システムのカスタマイズ
	virtual AttackConstants GetAttackConstants() override;
	virtual void GetAttackConfigs(AttackConfig configs[3]) override;
};

