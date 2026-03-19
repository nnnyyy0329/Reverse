#pragma once
#include "PlayerBase.h"

// 弾発射に関する定数
namespace BulletConstants
{
	constexpr float CONSUME_NORMAL_BULLET_ENERGY = 5.0f;	// 通常弾のエネルギー消費量
	constexpr float CONSUME_PIERCING_BULLET_ENERGY = 10.0f;	// 貫通弾のエネルギー消費量
}

// 弾発射設定
namespace BulletConstants
{
	const VECTOR RIGHT_ARM_SHOT_OFFSET = VGet(25, 80, 0);
	const VECTOR LEFT_ARM_SHOT_OFFSET = VGet(-15, 80, 0);
	constexpr int LIFE_TIME = 120;
};

// 通常弾設定
namespace NormalBulletConfig
{
	constexpr float RADIUS = 20.0f;
	constexpr float DAMAGE = 50.0f;
	constexpr float SPEED = 20.0f;
}

// 貫通弾設定
namespace PiercingBulletConfig
{
	constexpr float RADIUS = 15.0f;
	constexpr float DAMAGE = 30.0f;
	constexpr float SPEED = 25.0f;
}

// 前方宣言
class CameraManager;

/// @brief 弾発射プレイヤークラス
class BulletPlayer : public PlayerBase
{
public:

	BulletPlayer();
	virtual ~BulletPlayer();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	/// @brief デバッグ描画
	virtual void DebugRender()override;																		

	/// @brief 弾発射時間のデバッグ表示
	void DrawShootIntervalTime();

	/// @brief 被ダメージ処理
	/// 
	/// @param fDamage ダメージ量
	/// @param eType 攻撃の所有者タイプ
	/// @param attackInfo 攻撃コリジョン情報
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo) override;	

	/// @brief 弾による被ダメージ処理
	///
	/// @param fDamage ダメージ量
	/// @param char 弾のキャラタイプ
	/// @param bulletType 弾のタイプ
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara, BULLET_TYPE bulletType)override;

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig		GetPlayerConfig()		override;	// 設定を取得
	virtual PlayerAnimations	GetPlayerAnimation()	override;	// アニメーション設定を取得
	virtual RenderConfig		GetRenderConfig()		override;	// 描画設定を取得
	virtual DodgeConfig			GetDodgeConfig()		override;	// 回避設定を取得
	virtual BulletConfig		GetBulletConfig()		override;	// 弾発射設定を取得
	virtual BulletEffectConfig	GetBulletEffectConfig()	override;	// 弾演出設定を取得

	/* クラス設定 */

	// カメラマネージャーをセット

	/// @brief カメラマネージャーをセットする関数
	///
	/// @param cameraManager カメラマネージャーの共有ポインタ
	void SetCameraManager(std::shared_ptr<CameraManager>cameraManager){ _cameraManager = cameraManager; }	

private:

	/// @brief 弾発射処理
	void ProcessShoot()override;

	/// @brief 弾発射入力処理
	void ShootInput();

	/// @brief 発射状態へ移行処理
	void TransToShootState();

	/// @brief 発射状態の処理
	/// 
	/// @param isInputActive 発射入力がアクティブかどうか
	void UpdateByShootIntervalEnd(bool isInputActive);

	/// @brief 発射状態をリセット
	void ResetShootState();

	/// @brief 弾発射処理
	void ShootBullet();

	/// @brief 発射位置オフセットの取得
	///
	/// @return 発射位置オフセット
	VECTOR GetShootOffset()const;

	/// @brief エイム中かどうかで発射方向を決める
	///
	/// @return 発射方向
	VECTOR GetShootDirection()const;

	/// @brief 通常弾発射入力チェック
	///
	/// @return 通常弾発射入力があったらtrue、なければfalse
	bool IsShootNormalInput();

	/// @brief 貫通弾発射入力チェック
	///
	/// @return 貫通弾発射入力があったらtrue、なければfalse
	bool IsShootPiercingInput();

	/// @brief 発射間隔がマイナスかどうかをチェックする関数
	/// 
	/// @return 発射間隔がマイナスならtrue、そうでなければfalse
	bool IsShootIntervalNegative()const;

	/// @brief 弾のタイプをセットする関数
	///
	/// @param bulletType セットする弾のタイプ
	void SetBulletType(BULLET_TYPE bulletType);	


	BULLET_TYPE _currentBulletType;	// 現在の弾の種類

protected:

	std::weak_ptr<BulletManager>_bulletManager;		// 弾マネージャーの弱参照
	std::shared_ptr<CameraManager>_cameraManager;	// カメラマネージャーの共有ポインタ

	float _shootIntervalTimer;	// 発射間隔タイマー
	bool _bIsShootFromRightArm;	// 右腕から発射したかどうか
	bool _bIsReadyCompleted;	// 構えアニメーション完了フラグ
	bool _bWasShootKeyPressed;	// 前フレームで発射キーが押されていたか
};

