#include "StaminaManager.h"

// スタミナ関連定数のエイリアス
namespace SC = StaminaConstants;	

// スタミナ自動回復と自動消費に関する定数エイリアス
namespace ARC = AutoRecoveryConstants;

// シングルトン用メンバ初期化
StaminaManager* StaminaManager::_instance = nullptr;

void StaminaManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new StaminaManager();	// インスタンス作成
		_instance->Initialize();			// 初期化
	}
}

void StaminaManager::DestroyInstance()
{
	if(_instance)
	{
		_instance->Terminate();	// 終了
		delete _instance;		// インスタンス破棄
		_instance = nullptr;
	}
}

StaminaManager::StaminaManager()
{
	_currentStamina = SC::MAX_STAMINA;	// 現在のスタミナ量初期化
	minStamina = SC::MIN_STAMINA;		// スタミナの最小値初期化
	_maxStamina = SC::MAX_STAMINA;		// スタミナの最大値初期化
	_drainRate = SC::DRAIN_RATE;		// スタミナ消費率初期化
	_recoverRate = SC::RECOVER_RATE;	// スタミナ回復率初期化

	// 自動回復用変数初期化
	_isAutoRecovering = false;						// 自動回復中かどうか初期化
	_lastConsumeTime = 0.0f;						// 最後にスタミナを消費した時間初期化
	_autoRecoverDelay = ARC::AUTO_RECOVER_DELAY;	// 自動回復開始までの待機時間初期化

	// スタミナ消費アクション用変数初期化
	_dodgeCostStamina = SC::DODGE_COST_STAMINA;		// 回避に必要なスタミナ量初期化
	_shieldCostStamina = SC::SHIELD_COST_STAMINA;	// シールドに必要なスタミナ量の初期化
}

StaminaManager::~StaminaManager()
{

}

bool StaminaManager::Initialize()
{
	return true;
}

bool StaminaManager::Terminate()
{
	return true;
}

bool StaminaManager::Process()
{
	// 自動回復処理
	AutoRecoverStamina();

	return true;
}

bool StaminaManager::Render()
{
	return true;
}

void StaminaManager::AutoRecoverStamina()
{
	// スタミナが最大値未満の場合
	if(_currentStamina < _maxStamina)
	{
		_lastConsumeTime++;	// 最後にスタミナを消費してからの経過時間を更新

		// 自動回復待機時間経過後に回復開始
		if(_lastConsumeTime >= _autoRecoverDelay)
		{
			if(!_isAutoRecovering)
			{
				_isAutoRecovering = true;	// 自動回復開始
			}

			// スタミナ自動回復
			AddStamina(ARC::AUTO_RECOVER);
		}
	}
	// スタミナが最大値以上の場合
	else
	{
		_isAutoRecovering = false;	// 最大値に達したら自動回復終了
		_lastConsumeTime = 0.0f;	// 経過時間リセット
	}
}

void StaminaManager::AutoDrainStamina()
{
	
}

void StaminaManager::AddStamina(float stamina)
{
	// スタミナを回復率に応じて追加
	_currentStamina += stamina * _recoverRate;

	// 最大値を超えないように制限
	if (_currentStamina > _maxStamina)
	{
		_currentStamina = _maxStamina;
	}
}

void StaminaManager::ConsumeStamina(float stamina)
{
	_currentStamina -= stamina * _drainRate;

	// 最小値を下回らないように制限
	if (_currentStamina < minStamina)
	{
		_currentStamina = minStamina;
	}

	_isAutoRecovering = false;	// 自動回復停止
	_lastConsumeTime = 0.0f;	// 経過時間リセット
}

bool StaminaManager::CanDodgeByCost()
{
	return (_currentStamina >= _dodgeCostStamina);
}

bool StaminaManager::CanShieldByCost()
{
	return (_currentStamina >= _shieldCostStamina);
}

