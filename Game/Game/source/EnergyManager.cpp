#include "EnergyManager.h"

EnergyManager::EnergyManager()
{
    _currentEnergy = 0.0f;
    _maxEnergy = 100.0f;
}

EnergyManager::~EnergyManager()
{

}

bool EnergyManager::Initialize()
{
    return true;
}

bool EnergyManager::Terminate()
{
    return true;
}

bool EnergyManager::Process()
{
	return true;
}

bool EnergyManager::Render()
{
	return true;
}

// ダメージをエネルギーに変換する関数
void EnergyManager::ConvertDamageToEnergy(float damage)
{
    float convertEnergy = damage;
    AddEnergy(convertEnergy);   // エネルギー追加
}

// エネルギー追加関数
void EnergyManager::AddEnergy(float energy)
{

}

// エネルギー消費関数
void EnergyManager::ConsumeEnergy(float energy)
{

}
