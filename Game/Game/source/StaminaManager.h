/*****************************************
* file   StaminaManager.h
* brief  スタミナ管理クラス
* author 成田 悠真
* date   2026/02/06
******************************************/

#pragma once
#include "appframe.h"

// スタミナ関連定数
namespace StaminaConstants
{
	const float MIN_STAMINA			= 0.0f;		// スタミナの最小値
	const float MAX_STAMINA			= 100.0f;	// スタミナの最大値
	const float DRAIN_RATE			= 1.0f;		// スタミナ消費率
	const float RECOVER_RATE		= 1.0f;		// スタミナ回復率

	const float DODGE_COST_STAMINA	= 10.0f;	// 回避に必要なスタミナ量
	const float SHIELD_COST_STAMINA = 0.1f;		// シールドに必要なスタミナ量
}

// スタミナの自動回復と自動消費に関する定数
namespace AutoRecoveryConstants
{
	const float AUTO_RECOVER		= 1.0f;		// 自動回復量
	const float AUTO_RECOVER_RATE	= 0.5f;		// 自動回復率
	const float AUTO_DRAIN			= 3.0f;		// 自動消費量
	const float AUTO_DRAIN_RATE		= 1.5f;		// 自動消費率
	const float AUTO_RECOVER_DELAY	= 150.0f;	// 自動回復開始までの待機時間
}

/// @brief スタミナ管理クラス
class StaminaManager
{
public:

	/* シングルトン関数 */

	/// @brief インスタンス取得関数
	static StaminaManager* GetInstance()
	{
		static StaminaManager instance;	// 静的ローカル変数シングルトン
		return &instance;				// インスタンスのアドレスを返す
	}

	/// @brief インスタンス作成関数
	static void CreateInstance();

	/// @brief インスタンス破棄関数
	static void DestroyInstance();


	/* 基本関数 */

	bool Initialize();	// 初期化
	bool Terminate();	// 終了	
	bool Process();		// 更新
	bool Render();		// 描画


	/* スタミナ管理関数 */

	/// @brief スタミナの自動回復処理
	void AutoRecoverStamina();		

	/// @brief スタミナの自動消費処理
	void AutoDrainStamina();		

	/// @brief スタミナを追加する関数
	///
	/// @param stamina 追加するスタミナ量
	void AddStamina(float stamina);		
	
	/// @brief スタミナを消費する関数
	///
	/// @param stamina 消費するスタミナ量
	void ConsumeStamina(float stamina);	


	/* チェック関数 */

	/// @brief スタミナが回避に必要な量を満たしているかチェックする関数
	///
	/// @return 回避に必要なスタミナ量を満たしていればtrue、そうでなければfalse
	bool CanDodgeByCost();
	
	/// @brief スタミナがシールドに必要な量を満たしているかチェックする関数
	///
	/// @return シールドに必要なスタミナ量を満たしていればtrue、そうでなければfalse
	bool CanShieldByCost();	


	/* ゲッターセッター */

	float GetCurrentStamina() const { return _currentStamina; }			// 現在のスタミナ量取得
	void SetCurrentStamina(float stamina){ _currentStamina = stamina; } // 現在のスタミナ量設定

	float GetMaxStamina() const { return _maxStamina; }			// 最大のスタミナ量取得
	void SetMaxStamina(float stamina){ _maxStamina = stamina; } // 最大のスタミナ量設定

protected:

	// スタミナ管理変数
	float _currentStamina;	// 現在のスタミナ量
	float minStamina;		// 最小スタミナ量
	float _maxStamina;		// 最大のスタミナ量
	float _drainRate;		// スタミナ消費率
	float _recoverRate;		// スタミナ回復率

	// 自動回復用変数
	bool _isAutoRecovering;		// 自動回復中かどうか
	float _lastConsumeTime;		// 最後にスタミナを消費した時間
	float _autoRecoverDelay;	// 自動回復開始までの待機時間

	// スタミナ消費アクション用変数
	float _dodgeCostStamina;	// 回避に必要なスタミナ量
	float _shieldCostStamina;	// シールドに必要なスタミナ量

private:

	// コンストラクタとデストラクタ
	StaminaManager();
	virtual ~StaminaManager();

	// シングルトン用変数
	static StaminaManager* _instance;

};

