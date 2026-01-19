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

    // エネルギー変換関数
    void ConvertDamageToEnergy(float damage);   // ダメージをエネルギーに変換する関数

    // エネルギー管理関数
    void AddEnergy(float Energy);       // エネルギー追加関数
    void ConsumeEnergy(float energy);   // エネルギー消費関数

    // ゲッターセッター
    float GetCurrentEnergy() const { return _currentEnergy; }       // 現在のエネルギーの取得
    void SetCurrentEnergy(float energy){ _currentEnergy = energy; } // 現在のエネルギーの設定

    float GetMaxEnergy() const { return _maxEnergy; }       // 最大のエネルギーの取得
    void SetMaxEnergy(float energy){ _maxEnergy = energy; } // 最大のエネルギーの設定

protected:
    float _currentEnergy;       // 現在のエネルギー量
    float _maxEnergy;           // 最大のエネルギー量
	float _convertMultiplier;   // ダメージからエネルギーへの変換倍率

};

