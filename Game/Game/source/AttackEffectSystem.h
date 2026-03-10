#pragma once
#include "appframe.h"
#include "GeometryUtility.h"

// 攻撃演出設定構造体
struct AttackEffectConfig
{
	// エフェクト設定
	bool isActiveEffect;	// エフェクトを有効にするか
	std::string effectName;	// エフェクト名
	VECTOR effectOffset;	// エフェクト位置オフセット
	
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

// 攻撃の演出システムクラス
class AttackEffectSystem
{
public:
	// シングルトンアクセス
	static AttackEffectSystem* GetInstance();	// インスタンス取得
	static void CreateInstance();				// インスタンス生成
	static void DestroyInstance();				// インスタンス破棄

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	// 攻撃演出の処理関数
	void AttackEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir);

	// 攻撃エフェクト処理
	void ProcessEffect(const AttackEffectConfig& config, const VECTOR& pos, const VECTOR& dir);

	// 攻撃サウンド処理
	void ProcessSound(const AttackEffectConfig& config);

	// カメラシェイク処理
	void ProcessCameraShake(const AttackEffectConfig& config);

	// ヒットストップ処理
	void ProcessHitStop(const AttackEffectConfig& config);

	/* クラスの設定 */

	// ゲッターセッター
	const AttackEffectConfig& GetAttackEffectConfig() const { return _stcAttackEffectConfig; }
	void SetAttackEffectConfig(const AttackEffectConfig& config) { _stcAttackEffectConfig = config; }

private:

	// シングルトン用コンストラクタ・デストラクタ
	AttackEffectSystem();
	virtual ~AttackEffectSystem();

	// シングルトンインスタンス
	static AttackEffectSystem* _instance;

protected:
	AttackEffectConfig _stcAttackEffectConfig;	// 攻撃演出の設定

};

