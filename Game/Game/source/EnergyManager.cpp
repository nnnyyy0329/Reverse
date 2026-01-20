#include "EnergyManager.h"

namespace
{
	const float MIN_ENERGY = 0.0f;			// 最小エネルギー量
	const float MAX_ENERGY = 100.0f;		// 最大エネルギー量
	const float CONVERT_MULTIPLIER = 0.5f;	// デフォルト変換倍率
	const float SWITCH_COST_ENERGY = 50.0f;	// 切り替え可能エネルギー
}

// シングルトン用メンバ初期化
EnergyManager* EnergyManager::_instance = nullptr;

EnergyManager::EnergyManager()
{
	_currentEnergy = MIN_ENERGY;				// 初期エネルギー量
	_maxEnergy = MAX_ENERGY;					// 最大エネルギー量
	_convertMultiplier = CONVERT_MULTIPLIER;	// ダメージからエネルギーへの変換倍率
	_switchCostEnergy = SWITCH_COST_ENERGY;		// 切り替え可能エネルギー
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

// 切り替え可能かチェック
bool EnergyManager::CanSwitchPlayer()
{
	// 切り替えに必要なエネルギーが足りているか
	if(_currentEnergy >= _switchCostEnergy)
	{
		// 足りている
		return true;
	}

	return false;
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
		_currentEnergy = MIN_ENERGY;	// エネルギーを最大値に固定
	}
}

// インスタンス作成
void EnergyManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new EnergyManager();	// インスタンス作成
		_instance->Initialize();			// 初期化
	}
}

// インスタンス破棄
void EnergyManager::DestroyInstance()
{
	if(_instance != nullptr)
	{
		_instance->Terminate();	// 終了

		delete _instance;		// インスタンス破棄
	}
}

// デバッグ表示
void EnergyManager::DebugRender()
{
	DrawFormatString(10, 250, GetColor(0, 55, 0), "現在のエネルギー: %3.1f/100", _currentEnergy);
}
