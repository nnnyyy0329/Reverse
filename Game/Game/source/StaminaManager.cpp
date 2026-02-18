#include "StaminaManager.h"

namespace
{
	const float MIN_STAMINA = 0.0f;		// スタミナの最小値
	const float MAX_STAMINA = 100.0f;	// スタミナの最大値
	const float DRAIN_RATE = 1.0f;		// スタミナ消費率
	const float RECOVER_RATE = 1.0f;	// スタミナ回復率

	const float AUTO_RECOVER = 1.0f;			// 自動回復量
	const float AUTO_RECOVER_RATE = 0.5f;		// 自動回復率
	const float AUTO_DRAIN = 3.0f;				// 自動消費量
	const float AUTO_DRAIN_RATE = 1.5f;			// 自動消費率
	const float AUTO_RECOVER_DELAY = 150.0f;	// 自動回復開始までの待機時間

	const float DODGE_COST_STAMINA = 10.0f;	// 回避に必要なスタミナ量
	const float SHIELD_COST_STAMINA = 0.1f;	// シールドに必要なスタミナ量
}

// シングルトン用メンバ初期化
StaminaManager* StaminaManager::_instance = nullptr;

StaminaManager::StaminaManager()
{
	_currentStamina = MAX_STAMINA;	// 現在のスタミナ量初期化
	minStamina = MIN_STAMINA;		// スタミナの最小値初期化
	_maxStamina = MAX_STAMINA;		// スタミナの最大値初期化
	_drainRate = DRAIN_RATE;		// スタミナ消費率初期化
	_recoverRate = RECOVER_RATE;	// スタミナ回復率初期化

	// 自動回復用変数初期化
	_isAutoRecovering = false;				// 自動回復中かどうか初期化
	_lastConsumeTime = 0.0f;				// 最後にスタミナを消費した時間初期化
	_autoRecoverDelay = AUTO_RECOVER_DELAY;	// 自動回復開始までの待機時間初期化

	// スタミナ消費アクション用変数初期化
	_dodgeCostStamina = DODGE_COST_STAMINA;		// 回避に必要なスタミナ量初期化
	_shieldCostStamina = SHIELD_COST_STAMINA;	// シールドに必要なスタミナ量の初期化
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

// 自動回復関数
void StaminaManager::AutoRecoverStamina()
{
	// スタミナが最大値未満の場合に回復処理
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

			// スタミナ回復
			AddStamina(AUTO_RECOVER);
		}
	}
	else
	{
		_isAutoRecovering = false;	// 最大値に達したら自動回復終了
		_lastConsumeTime = 0.0f;	// 経過時間リセット
	}
}

// 自動消費関数
void StaminaManager::AutoDrainStamina()
{
	
}

// スタミナ追加関数
void StaminaManager::AddStamina(float stamina)
{
	_currentStamina += stamina * _recoverRate;

	// 最大値を超えないように制限
	if (_currentStamina > _maxStamina)
	{
		_currentStamina = _maxStamina;
	}
}

// スタミナ消費関数
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

// 回避可能かチェック
bool StaminaManager::CanDodge()
{
	return (_currentStamina >= _dodgeCostStamina);
}

// シールド可能かチェック
bool StaminaManager::CanShield()
{
	return (_currentStamina >= _shieldCostStamina);
}

// インスタンス作成
void StaminaManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new StaminaManager();	// インスタンス作成
		_instance->Initialize();			// 初期化
	}
}

// インスタンス破棄
void StaminaManager::DestroyInstance()
{
	if(_instance)
	{
		_instance->Terminate();	// 終了
		delete _instance;		// インスタンス破棄
		_instance = nullptr;
	}
}