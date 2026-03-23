// 担当 : 成田

#pragma once
#include "appframe.h"

// エネルギー情報定数
namespace EnergyDefaultConstants
{
    constexpr float MIN_ENERGY = 0.0f;				// 最小エネルギー量
    constexpr float MAX_ENERGY = 300.0f;			// 最大エネルギー量
    constexpr float CONVERT_MULTIPLIER = 1.0f;		// デフォルト変換倍率
    constexpr float CONSUME_MULTIPLIER = 1.0f;		// デフォルト消費倍率
    constexpr float SWITCH_COST_ENERGY = 100.0f;	// 切り替え可能エネルギー
    constexpr float SWITCH_KEEP_ENERGY = 0.01f;		// 切り替え維持エネルギー
}

// エネルギー獲得量定数
namespace EnergyGainConstants
{
    constexpr float EVADE_ATTACK = 10.0f;
    constexpr float ABSORB = 1.0f;
}

class EnergyManager
{
public:

    // シングルトン
    static EnergyManager* GetInstance()	// インスタンス取得
    {
        // 静的ローカル変数シングルトン
        static EnergyManager instance;

        return &instance;
    }
    static void CreateInstance();	// インスタンス作成
    static void DestroyInstance();	// インスタンス破棄

    bool Initialize();
    bool Terminate();
    bool Process();
    bool Render();
    void DebugRender();


    /* エネルギー管理関数 */

    // ダメージをエネルギーに変換する関数
    void ConvertDamageToEnergy(float damage);

    // ダメージを消費エネルギーに変換する関数
    void ConvertDamageToConsumeEnergy(float damage);

    // エネルギー追加関数
    void AddEnergy(float Energy);                   

    // エネルギー消費関数
    void ConsumeEnergy(float energy);

    // 切り替え可能かチェック
    bool CanSwitchPlayer();

    // 切り替え維持可能かチェック
    bool CanKeepSwitchPlayer();


    /* ゲッターセッター */

    // 現在のエネルギーの取得
    float GetCurrentEnergy() const { return _currentEnergy; }       

    // 現在のエネルギーの設定
    void SetCurrentEnergy(float energy){ _currentEnergy = energy; } 

    // 最大のエネルギーの取得
    float GetMaxEnergy() const { return _maxEnergy; }       

    // 最大のエネルギーの設定
    void SetMaxEnergy(float energy){ _maxEnergy = energy; } 

    // 切り替えエネルギーの取得
    float GetSwitchCostEnergy() const { return _switchCostEnergy; } 

    // 切り替え維持エネルギーの取得
	float GetSwitchKeepEnergy() const { return _switchKeepEnergy; }   

    // 攻撃回避時のエネルギー獲得量
    float GetEvadeAttackEnergy()const;

    // 吸収時のエネルギー獲得量
    float GetAbsorbEnergy()const;

private:
    
    // コンストラクタ、デストラクタ
    EnergyManager();
    virtual ~EnergyManager();

    // シングルトン用変数
    static EnergyManager* _instance;

protected:

    float _currentEnergy;               // 現在のエネルギー量
    float _maxEnergy;                   // 最大のエネルギー量
	float _convertMultiplier;           // ダメージからエネルギーへの変換倍率
	float _consumeConvertMultiplier;    // ダメージから消費エネルギーへの変換倍率
    float _switchCostEnergy;            // 切り替えに必要なエネルギー量
	float _switchKeepEnergy;            // 切り替え状態維持に必要なするエネルギー量

};

