#include "EnergyManager.h"

// エネルギー情報定数のエイリアス
namespace EDC = EnergyDefaultConstants;

// エネルギー獲得量定数のエイリアス
namespace EGC = EnergyGainConstants;

// エネルギー消費量定数のエイリアス
namespace ECC = EnergyConsumeConstants;

// シングルトン用メンバ初期化
EnergyManager* EnergyManager::_instance = nullptr;

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

EnergyManager::EnergyManager()
{
	_currentEnergy				= EDC::MIN_ENERGY;			// 初期エネルギー量
	_maxEnergy					= EDC::MAX_ENERGY;			// 最大エネルギー量
	_convertMultiplier			= EDC::CONVERT_MULTIPLIER;	// ダメージからエネルギーへの変換倍率
	_consumeConvertMultiplier	= EDC::CONSUME_MULTIPLIER;	// ダメージから消費エネルギーへの変換倍率
	_switchCostEnergy			= EDC::SWITCH_COST_ENERGY;	// 切り替え可能エネルギー
	_switchKeepEnergy			= EDC::SWITCH_KEEP_ENERGY;	// 切り替え維持エネルギー
}

EnergyManager::~EnergyManager()
{

}

bool EnergyManager::Initialize()
{
	_currentEnergy = 0.0f;	// エネルギー初期化

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

void EnergyManager::ConvertDamageToEnergy(float damage)
{
	// ダメージをエネルギーに変換
	float convertEnergy = damage * _convertMultiplier;	

	// エネルギー追加
	AddEnergy(convertEnergy);							
}

void EnergyManager::ConvertDamageToConsumeEnergy(float damage)
{
	// ダメージを消費エネルギーに変換
	float consumeEnergy = damage * _consumeConvertMultiplier;	

	// エネルギー消費
	ConsumeEnergy(consumeEnergy);								
}

void EnergyManager::AddEnergy(float energy)
{
	// エネルギーを追加
	_currentEnergy += energy;

	// 最大エネルギーを超えた場合の処理
	if (_currentEnergy > _maxEnergy)
	{
		// 最大エネルギーを超えないようにする
		_currentEnergy = _maxEnergy;	
	}
}

void EnergyManager::ConsumeEnergy(float energy)
{
	// エネルギーを消費
	_currentEnergy -= energy;

	// エネルギーが0未満にならないようにする
	if (_currentEnergy < EDC::MIN_ENERGY)
	{
		// エネルギーを最大値に固定
		_currentEnergy = EDC::MIN_ENERGY;	
	}
}

void EnergyManager::DebugRender()
{
	// エネルギー量を画面に表示
	DrawFormatString(500, 10, GetColor(255, 255, 255), "現在のエネルギー: %3.1f/%3.1f", _currentEnergy, _maxEnergy);
}
