#include "EnergyManager.h"

namespace
{
	const float MIN_ENERGY = 0.0f;					// 最小エネルギー量
	const float MAX_ENERGY = 100.0f;				// 最大エネルギー量（デフォルト値）
	const float DEFAULT_CONVERT_MULTIPLIER = 1.5f;	// デフォルト変換倍率
}

EnergyManager::EnergyManager()
{
	_currentEnergy = MIN_ENERGY;						// 初期エネルギー量
	_maxEnergy = MAX_ENERGY;							// 最大エネルギー量
	_convertMultiplier = DEFAULT_CONVERT_MULTIPLIER;	// ダメージからエネルギーへの変換倍率
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
	float convertEnergy = damage * _convertMultiplier;	// ダメージをエネルギーに変換
    AddEnergy(convertEnergy);							// エネルギー追加
}

// エネルギー追加関数
void EnergyManager::AddEnergy(float energy)
{
	_currentEnergy += energy;

	// 最大エネルギーを超えた場合の処理
	if (_currentEnergy > _maxEnergy)
	{
		_currentEnergy = _maxEnergy;	// 最大エネルギーを超えないようにする
	}
}

// エネルギー消費関数
void EnergyManager::ConsumeEnergy(float energy)
{
	_currentEnergy -= energy;

	// エネルギーが0未満にならないようにする
	if (_currentEnergy < MIN_ENERGY)
	{
		_currentEnergy = MIN_ENERGY;	// エネルギーが0未満にならないようにする
	}
}
