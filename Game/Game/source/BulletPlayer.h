#pragma once
#include "PlayerBase.h"

// 前方宣言
class CameraManager;

// 弾発射プレイヤー
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
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo) override;	// 被ダメージ処理
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										// 弾による被ダメージ処理

	// 弾発射時間のデバッグ表示
	void DrawShootIntervalTime();												

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig		GetPlayerConfig()		override;	// 設定を取得
	virtual PlayerAnimations	GetPlayerAnimation()	override;	// アニメーション設定を取得
	virtual RenderConfig		GetRenderConfig()		override;	// 描画設定を取得
	virtual DodgeConfig			GetDodgeConfig()		override;	// 回避設定を取得
	virtual BulletConfig		GetBulletConfig()		override;	// 弾発射設定を取得
	virtual BulletEffectConfig	GetBulletEffectConfig()	override;	// 弾演出設定を取得

	// クラス設定
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

	// 発射位置オフセットの取得
	VECTOR GetShootOffset()const;

	// エイム中かどうかで発射方向を決める
	VECTOR GetShootDirection()const;

	// 弾発射入力チェック
	bool IsShootInput()const;

	// 弾の種類切り替え入力チェック
	bool IsChangeBulletTypeInput()const;

	// 発射間隔がマイナスかどうか
	bool IsShootIntervalNegative()const;

protected:
	std::weak_ptr<BulletManager>_bulletManager;
	std::shared_ptr<CameraManager>_cameraManager;

	float _shootIntervalTimer;	// 発射間隔タイマー
	bool _bIsShootFromRightArm;	// 右腕から発射したかどうか
	bool _bIsReadyCompleted;	// 構えアニメーション完了フラグ
	bool _bWasShootKeyPressed;	// 前フレームで発射キーが押されていたか
};

