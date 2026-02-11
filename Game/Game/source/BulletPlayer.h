#pragma once
#include "PlayerBase.h"

struct BulletConfig
{

};

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

	void ProcessShoot()override;		// 基底クラスの弾発射関数のオーバーライド
	void ShootByInput();				// 入力確認
	//void ShootIntervalUpdate(bool key);	// 発射間隔更新
	void ShootBullet();					// 弾の発射

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;				// 設定を取得
	virtual PlayerAnimations GetPlayerAnimation() override;			// アニメーション設定を取得
	virtual RenderConfig GetRenderConfig() override;				// 描画設定を取得
	virtual DodgeConfig GetDodgeConfig() override;					// 回避設定を取得
	virtual ShieldConfig GetShieldConfig() override;				// シールド設定を取得
	virtual AttackConstants GetAttackConstants() override;			// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[]) override;	// 攻撃設定を取得

	// クラス設定
	void SetBulletManager(std::shared_ptr<BulletManager>bulletManager){ _bulletManager = bulletManager; }	// マネージャーをセット

protected:
	std::weak_ptr<BulletManager>_bulletManager;

	float _shootIntervalTimer;	// 発射間隔タイマー
	bool _bIsShootFromRightArm;	// 右腕から発射したかどうか

};

