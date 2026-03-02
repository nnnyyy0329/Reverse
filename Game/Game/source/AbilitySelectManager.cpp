#include "AbilitySelectManager.h"
#include "AbilitySelectScreen.h"
#include "PlayerManager.h"

bool AbilitySelectManager::Initialize()
{
	_abilitySelectScreen = nullptr;	// 能力選択画面
	_playerManager = nullptr;		// プレイヤーマネージャー

	return true;
}

bool AbilitySelectManager::Terminate()
{
	return true;
}

bool AbilitySelectManager::Process()
{
	// アビリティ選択処理
	AbilitySelection();

	return true;
}

// 能力選択画面のセット
void AbilitySelectManager::SetAbilitySelectScreen(std::shared_ptr<AbilitySelectScreen> selectScreen)
{
	_abilitySelectScreen = selectScreen;
}

// プレイヤーマネージャーのセット
void AbilitySelectManager::SetPlayerManager(std::shared_ptr<PlayerManager> playerManager)
{
	_playerManager = playerManager;
}

// アビリティ選択処理
void AbilitySelectManager::AbilitySelection()
{
	if(!_abilitySelectScreen || !_playerManager){ return; }	// クラスがセットされていない場合は処理しない

	if(!IsSwitchPlayerByEnergy()){ return; }	// エネルギーによる切り替えができない場合は処理しない

	// 選択状態が選択確定の場合
	if(_abilitySelectScreen->GetSelectionState() == SelectionState::SELECT_COMPLETED)
	{
		// 選択されたアビリティを取得
		ABILITY_TYPE selectedAbility = _abilitySelectScreen->GetSelectedAbility();

		// アビリティに応じてプレイヤーを切り替える
		_playerManager->SwitchPlayerByAbility(selectedAbility);

		// 選択完了処理
		_abilitySelectScreen->CompleteSelection();
	}
}

// エネルギーによるプレイヤー切り替えかどうかのチェック
bool AbilitySelectManager::IsSwitchPlayerByEnergy()
{
	EnergyManager* energyManager = EnergyManager::GetInstance();

	// エネルギーによる切り替えが可能かチェック
	if(energyManager->CanSwitchPlayer())
	{
		return true;	// エネルギーによる切り替えが可能
	}

	return false;
}
