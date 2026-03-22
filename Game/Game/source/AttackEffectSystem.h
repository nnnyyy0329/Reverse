#pragma once
#include "appframe.h"
#include "GeometryUtility.h"
#include "AnimManager.h"

// 前方宣言
class CharaBase;

// @brief エフェクトのアタッチタイプ列挙型
enum class EFFECT_ATTACH_TYPE
{
	NONE,				
	LEFT_ARM,			///< 左腕フレーム位置
	RIGHT_ARM,			///< 右腕フレーム位置
	CHARACTER_OFFSET,	///< キャラクター位置 + オフセット
	_EOT_,
};

/// @brief 攻撃の演出設定構造体
struct AttackEffectConfig
{
	// エフェクト設定
	bool isActiveEffect;			// エフェクトを有効にするか
	std::string effectName;			// エフェクト名
	VECTOR effectOffset;			// エフェクト位置オフセット
	VECTOR effectRotation;			// エフェクト回転
	EFFECT_ATTACH_TYPE attachType;	// エフェクトのアタッチタイプ

	// サウンド設定
	bool isActiveSound;		// サウンドを有効にするか
	std::string soundName;	// サウンド名

	// カメラシェイク設定
	bool isActiveCameraShake;	// カメラシェイクを有効にするか
	float cameraShakeMagnitude;	// カメラシェイクの強さ
	float cameraShakeDuration;	// カメラシェイクの持続時間

	// ヒットストップ設定
	bool isActiveHitStop;		// ヒットストップを有効にするか
	float hitStopDuration;		// ヒットストップの持続時間
};

/// @brief エフェクトのインスタンス情報構造体
struct EffectInstanceInfo
{
	int effectHandle;	// エフェクトのハンドル
	VECTOR currentPos;	// エフェクトの現在位置
};

/// @brief 追跡するエフェクトの情報構造体
struct TrackedEffectInfo
{
	int effectHandle;				// エフェクトハンドル
	int attachFrameIndex;			// アタッチするフレームインデックス
	VECTOR effectOffset;			// エフェクトのオフセット
	VECTOR effectRotation;			// エフェクトの回転
	AnimManager* animManager;		// アニメーションマネージャー
	std::weak_ptr<CharaBase> owner;	// 所有者キャラの弱参照
	bool useOwnerDirection = false;	// 所有者の向きを基準とするか(最初から false )
	EFFECT_ATTACH_TYPE attachType;	// エフェクトのアタッチタイプ
};

/// @brief 攻撃の演出システムクラス
class AttackEffectSystem
{
public:

	/* シングルトンアクセス */

	/// @brief インスタンス取得処理
	static AttackEffectSystem* GetInstance();	

	/// @brief インスタンス生成処理
	static void CreateInstance();				

	/// @brief インスタンス破棄処理
	static void DestroyInstance();				


	/* 基本処理 */

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	/* 攻撃演出処理関数 */

	/// @brief 攻撃エフェクト再生処理
	///
	/// @param config 攻撃演出設定
	/// @param pos エフェクト再生位置
	/// @param dir エフェクトの向き（攻撃方向）
	///	
	/// @return 再生したエフェクトのハンドル
	int PlayEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir);

	/// @brief 追跡エフェクト再生処理
	///
	/// @param config 攻撃演出設定
	/// @param initialPos エフェクトの初期位置
	/// @param dir エフェクトの向き（攻撃方向）
	/// @param frameIndex 追跡するフレームのインデックス
	/// @param animManager 追跡するアニメーションマネージャーの共有ポインタ
	/// @param owner エフェクトの所有者キャラの共有ポインタ
	/// 
	/// @return 再生したエフェクトのハンドル
	int PlayTrackedEffect
	(
		const AttackEffectConfig& config,
		const VECTOR& initialPos,		
		const VECTOR& dir,
		int frameIndex,
		AnimManager* animManager,
		std::shared_ptr<CharaBase> owner
	);

	/// @brief エフェクト停止処理
	///
	/// @param effectHandle 停止するエフェクトのハンドル
	void StopEffect(int effectHandle);

	/// @brief 追跡エフェクトの位置計算処理
	///
	/// @param info 追跡エフェクトの情報構造体
	VECTOR UpdateCalculatePos(const TrackedEffectInfo& info);

	/// @brief 追跡エフェクトの回転計算処理
	///
	/// @param info 追跡エフェクトの情報構造体
	VECTOR UpdateCalculateRot(const TrackedEffectInfo& info);

	/// @brief エフェクトの演出処理
	///
	/// @param handle エフェクトのハンドル
	/// @param dir エフェクトの向き（攻撃方向）
	/// @param customRotation エフェクトのカスタムの回転量
	void ProcessEffect(int handle, const VECTOR& dir, const VECTOR& customRotation);

	/// @brief 攻撃サウンド演出処理
	///
	/// @param config 攻撃演出設定
	void ProcessSound(const AttackEffectConfig& config);

	/// @brief カメラシェイク演出処理
	///
	/// @param config 攻撃演出設定
	void ProcessCameraShake(const AttackEffectConfig& config);

	/// @brief ヒットストップ演出処理
	///
	/// @param config 攻撃演出設定
	void ProcessHitStop(const AttackEffectConfig& config);

	/* クラスの設定 */

private:

	// シングルトン用コンストラクタ・デストラクタ
	AttackEffectSystem();
	virtual ~AttackEffectSystem();

	// シングルトンインスタンス
	static AttackEffectSystem* _instance;


	/// @brief 追跡エフェクトの位置更新処理
	void UpdateTrackedEffects();


	// エフェクト管理関係
	std::map<int, EffectInstanceInfo> _activeEffects;	// アクティブなエフェクトの管理マップ
	std::map<int, TrackedEffectInfo> _trackedEffects;	// 追跡するエフェクトの管理マップ

protected:

};

