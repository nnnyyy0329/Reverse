// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "AttackBase.h"

class InteriorPlayer : public PlayerBase
{
public:
	InteriorPlayer();
	virtual ~InteriorPlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	virtual void DebugRender()override;									// デバッグ描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType) override;	// 被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;
	virtual PlayerAnimation GetPlayerAnimation() override;
	virtual RenderConfig GetRenderConfig() override;

	/*****ゲッターセッター*****/

protected:
	// 攻撃システムのカスタマイズ
	virtual AttackConstants GetAttackConstants() override;
	virtual void GetAttackConfigs(AttackConfig configs[5]) override;

};

