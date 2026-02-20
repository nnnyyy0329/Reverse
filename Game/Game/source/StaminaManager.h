#pragma once
#include "appframe.h"

// スタミナ管理クラス
class StaminaManager
{
public:
	StaminaManager();
	~StaminaManager();
	bool Initialize();	// 初期化
	bool Terminate();	// 終了	
	bool Process();		// 更新
	bool Render();		// 描画

	// スタミナ管理関数
	void AutoRecoverStamina();			// 自動回復関数
	void AutoDrainStamina();			// 自動消費関数
	void AddStamina(float stamina);		// スタミナ追加関数
	void ConsumeStamina(float stamina);	// スタミナ消費関数

	// チェック関数
	bool CanDodge();	// 回避可能かチェック
	bool CanShield();	// シールド可能かチェック

	// ゲッターセッター
	float GetCurrentStamina() const { return _currentStamina; }			// 現在のスタミナ量取得
	void SetCurrentStamina(float stamina){ _currentStamina = stamina; } // 現在のスタミナ量設定

	float GetMaxStamina() const { return _maxStamina; }			// 最大のスタミナ量取得
	void SetMaxStamina(float stamina){ _maxStamina = stamina; } // 最大のスタミナ量設定

	// シングルトン
	static StaminaManager* GetInstance()	// インスタンス取得
	{
		// 静的ローカル変数シングルトン
		static StaminaManager instance;

		return &instance;
	}
	static void CreateInstance();	// インスタンス作成
	static void DestroyInstance();	// インスタンス破棄

protected:
	// シングルトン用変数	
	static StaminaManager* _instance; 

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
};

