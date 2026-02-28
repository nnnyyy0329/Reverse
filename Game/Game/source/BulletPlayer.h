#pragma once
#include "PlayerBase.h"

class BulletManager;
class CameraManager;

class BulletPlayer : public PlayerBase
{
public:
	BulletPlayer();
	virtual ~BulletPlayer();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	// 共通関数のオーバーライド
	virtual void DebugRender()override;																		// デバッグ描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										// 弾による被ダメージ処理

	// オフセット位置をワールド座標に変換
	VECTOR TransOffsetToWorld(const VECTOR& offset, const VECTOR& playerDir);	

	// 弾発射時間のデバッグ表示
	void DrawShootIntervalTime();												

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig GetPlayerConfig() override;				// 設定を取得
	virtual PlayerAnimations GetPlayerAnimation() override;			// アニメーション設定を取得
	virtual RenderConfig GetRenderConfig() override;				// 描画設定を取得
	virtual DodgeConfig GetDodgeConfig() override;					// 回避設定を取得
	virtual ShieldConfig GetShieldConfig() override;				// シールド設定を取得
	virtual AttackConstants GetAttackConstants()const override;		// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[]) override;	// 攻撃設定を取得
	virtual AreaAttackConfig GetAreaAttackConfig() override;		// 範囲攻撃設定を取得

	// クラス設定
	void SetBulletManager(std::shared_ptr<BulletManager>bulletManager){ _bulletManager = bulletManager; }	// マネージャーをセット
	void SetCameraManager(std::shared_ptr<CameraManager>cameraManager){ _cameraManager = cameraManager; }	// カメラマネージャーをセット

private:
	// 基底クラスの弾発射関数のオーバーライド
	void ProcessShoot()override;

	// 弾発射の入力処理
	void ShootInput();

	// 弾の発射
	void ShootBullet();

	// エイムモードの処理
	void ProcessAimMode(bool aimKey);

	// 弾発射入力チェック
	bool IsShootInput()const;

protected:
	std::weak_ptr<BulletManager>_bulletManager;
	std::shared_ptr<CameraManager>_cameraManager;

	float _shootIntervalTimer;	// 発射間隔タイマー
	bool _bIsShootFromRightArm;	// 右腕から発射したかどうか
	bool _bIsReadyCompleted;	// 構えアニメーション完了フラグ
	bool _bWasShootKeyPressed;	// 前フレームで発射キーが押されていたか
};

