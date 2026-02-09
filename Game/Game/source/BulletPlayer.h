#pragma once
#include "PlayerBase.h"

class BulletManager;

class BulletPlayer : public PlayerBase
{
public:
	BulletPlayer();
	virtual ~BulletPlayer();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	virtual void DebugRender()override;									// デバッグ描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理

	void ShootByInput();	// 入力確認
	void ShootBullet();		// 弾の発射

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;		// 設定を取得
	virtual PlayerAnimation GetPlayerAnimation() override;	// アニメーション設定を取得
	virtual RenderConfig GetRenderConfig() override;		// 描画設定を取得
	virtual DodgeConfig GetDodgeConfig() override;			// 回避設定を取得
	virtual ShieldConfig GetShieldConfig() override;		// シールド設定を取得

	// クラス設定
	void SetBulletManager(std::shared_ptr<BulletManager>bulletManager){ _bulletManager = bulletManager; }	// マネージャーをセット

protected:
	// 攻撃システムのカスタマイズ
	virtual AttackConstants GetAttackConstants() override;
	virtual void GetAttackConfigs(AttackConfig configs[]) override;

	std::weak_ptr<BulletManager>_bulletManager;

};

