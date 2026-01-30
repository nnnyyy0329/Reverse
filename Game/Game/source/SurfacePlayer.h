// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "AttackBase.h"

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
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType) override;	// 被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;
	virtual PlayerAnimation GetPlayerAnimation() override;
	virtual RenderConfig GetRenderConfig() override;

	/*****ゲッターセッター*****/

protected:
	// 攻撃システムのカスタマイズ
	virtual AttackConstants GetAttackConstants() override;
	virtual void GetAttackConfigs(AttackConfig configs[3]) override;
};

