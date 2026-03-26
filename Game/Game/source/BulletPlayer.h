#pragma once
#include "PlayerBase.h"

// 弾プレイヤー用定数
namespace BulletPlayerConstants
{
	constexpr float DAMAGE_MULTIPLIER = 0.75f;	// 弾プレイヤー専用のダメージ倍率
}

// 通常弾設定定数
namespace NormalBulletConfig
{
	constexpr float RADIUS = 20.0f;		// 弾半径
	constexpr float DAMAGE = 100.0f;		// 弾のダメージ
	constexpr float SPEED = 20.0f;		// 弾の速度
	constexpr float LIFE_TIME = 60.0f;	// 弾の寿命
}

// 貫通弾設定定数
namespace PiercingBulletConfig
{
	constexpr float RADIUS = 15.0f;		// 弾半径
	constexpr float DAMAGE = 10.0f;		// 弾のダメージ
	constexpr float SPEED = 30.0f;		// 弾の速度
	constexpr float LIFE_TIME = 120.0f;	// 弾の寿命
}

// 弾発射設定定数
namespace BulletShootConstants
{
	const VECTOR RIGHT_ARM_SHOT_OFFSET = VGet(25, 80, 0);	// 右腕発射位置オフセット
	const VECTOR LEFT_ARM_SHOT_OFFSET = VGet(-15, 80, 0);	// 左腕発射位置オフセット
}

// 弾発射リコイル設定定数
namespace BulletRecoilConstants
{
	constexpr float RECOIL_MOVE_STRENGTH = 5.0f;	// リコイル移動の強さ
	constexpr float RECOIL_MOVE_DECAY = 0.8f;		// リコイル移動の減衰率
	constexpr int RECOIL_MOVE_DIRECTION = -1;		// リコイル移動方向
}

// 前方宣言
class CameraManager;

/// @brief 弾発射プレイヤークラス
class BulletPlayer : public PlayerBase
{
public:

	BulletPlayer();
	virtual ~BulletPlayer();



	/* 基本関数 */

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
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE ownerType, const AttackCollision& attackInfo) override;

	/// @brief 弾による被ダメージ処理
	///
	/// @param fDamage ダメージ量
	/// @param char 弾のキャラタイプ
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										



	/* クラス設定 */

	/// @brief カメラマネージャーをセットする関数
	///
	/// @param cameraManager カメラマネージャーの共有ポインタ
	void SetCameraManager(std::shared_ptr<CameraManager>cameraManager){ _cameraManager = cameraManager; }	



	/* ゲッターセッター */

	/// @brief 弾のタイプをセットする関数
	///
	/// @param bulletType セットする弾のタイプ
	void SetBulletType(BULLET_TYPE bulletType);


private:

	/* 仮想関数のオーバーライド */

	/// @brief プレイヤー設定を取得
	///
	/// @return プレイヤー設定構造体
	virtual PlayerConfig		GetPlayerConfig()		override;

	/// @brief プレイヤーアニメーション設定を取得
	///
	/// @return プレイヤーアニメーション設定構造体
	virtual PlayerAnimations	GetPlayerAnimation()	override;

	/// @brief 描画設定を取得
	///
	/// @return 描画設定構造体
	virtual RenderConfig		GetRenderConfig()		override;

	/// @brief 回避設定を取得
	///
	/// @return 回避設定構造体
	virtual DodgeConfig			GetDodgeConfig()		override;

	/// @brief 弾発射設定を取得
	///	
	/// @return 弾発射設定構造体
	virtual BulletConfig		GetBulletConfig()		override;

	/// @brief 弾演出設定を取得
	///
	/// @return 弾演出設定構造体
	virtual BulletEffectConfig	GetBulletEffectConfig()	override;	



	/* 弾発射処理 */

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

	/// @brief 弾発射サウンド再生処理
	///
	/// @param bulletType サウンド再生する弾タイプ
	void ShootSoundPlay(BULLET_TYPE bulletType);

	/// @brief 発射リコイル処理
	void ShootRecoilMove();

	/// @brief リコイル移動の更新処理
	void UpdateRecoilPos();

	/// @brief エイムカメラの角度更新処理
	void UpdateAimCameraAngle();

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


protected:

	BULLET_TYPE _currentBulletType;	// 現在の弾の種類

	std::weak_ptr<BulletManager>_bulletManager;		// 弾マネージャーの弱参照
	std::shared_ptr<CameraManager>_cameraManager;	// カメラマネージャーの共有ポインタ

	VECTOR _vRecoilVelocity;	// リコイル移動の速度
	float _shootIntervalTimer;	// 発射間隔タイマー
	bool _bIsShootFromRightArm;	// 右腕から発射したかどうか
	bool _bIsReadyCompleted;	// 構えアニメーション完了フラグ
	bool _bWasShootKeyPressed;	// 前フレームで発射キーが押されていたか
};

