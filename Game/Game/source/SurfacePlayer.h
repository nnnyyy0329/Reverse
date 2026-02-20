// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "PlayerAbsorbAttackSystem.h"

// 表プレイヤー
class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	virtual void DebugRender()override;																		// デバッグ情報描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										// 弾による被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig		GetPlayerConfig()			override;	// 設定を取得
	virtual PlayerAnimations	GetPlayerAnimation()		override;	// アニメーション設定を取得
	virtual RenderConfig		GetRenderConfig()			override;	// 描画設定を取得
	virtual DodgeConfig			GetDodgeConfig()			override;	// 回避設定を取得
	virtual ShieldConfig		GetShieldConfig()			override;	// シールド設定を取得
	virtual AttackConstants		GetAttackConstants()const	override;	// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[3])	override;	// 攻撃設定を取得
	const PlayerAbsorbAttackSystem* GetAbsorbAttackSystem()const;		// 吸収攻撃システム取得

	/*****ゲッターセッター*****/

private:
	// 吸収攻撃システム
	std::unique_ptr<PlayerAbsorbAttackSystem> _absorbAttackSystem;

	// 吸収攻撃関連
	void MakeAbsorbSystem();										// 吸収攻撃システム生成
	void InitializeAbsorbSystem();									// 吸収攻撃システム初期化
	void ProcessAbsorbSystem();										// 吸収攻撃システム処理
	AbsorbConfig GetAbsorbConfig();									// 吸収攻撃設定取得

	void AbsorbSystemDebugRender();	// 吸収攻撃システムデバッグ描画

protected:

};

