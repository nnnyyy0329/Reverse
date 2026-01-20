// 担当 : 成田

#pragma once
#include "appframe.h"

class EnergyManager
{
public:
    EnergyManager();
    virtual ~EnergyManager();

    bool Initialize();
    bool Terminate();
    bool Process();
    bool Render();

    // エネルギー管理関数
    bool CanSwitchPlayer();                     // 切り替え可能かチェック
    void ConvertDamageToEnergy(float damage);   // エネルギー変換関数
    void AddEnergy(float Energy);               // エネルギー追加関数
    void ConsumeEnergy(float energy);           // エネルギー消費関数

    // デバッグ表示
    void DebugRender();

    // ゲッターセッター
    float GetCurrentEnergy() const { return _currentEnergy; }       // 現在のエネルギーの取得
    void SetCurrentEnergy(float energy){ _currentEnergy = energy; } // 現在のエネルギーの設定

    float GetMaxEnergy() const { return _maxEnergy; }       // 最大のエネルギーの取得
    void SetMaxEnergy(float energy){ _maxEnergy = energy; } // 最大のエネルギーの設定

    float GetSwitchEnergyCost() const { return _switchCostEnergy; } // 切り替えエネルギーの取得

    // シングルトン
    static EnergyManager* GetInstance()	// インスタンス取得
    {
        // 静的ローカル変数シングルトン
        static EnergyManager instance;

        return &instance;
    }
    static void CreateInstance();	// インスタンス作成
    static void DestroyInstance();	// インスタンス破棄

protected:
    float _currentEnergy;       // 現在のエネルギー量
    float _maxEnergy;           // 最大のエネルギー量
	float _convertMultiplier;   // ダメージからエネルギーへの変換倍率
    float _switchCostEnergy;    // 切り替えに必要なエネルギー量

    // シングルトン用変数
    static EnergyManager* _instance;
};

