#pragma once
#include "appframe.h"

// 前方宣言
class AbilitySelectScreen;
class PlayerManager;

class AbilitySelectManager
{
public:
	bool Initialize();
	bool Terminate();
	bool Process();

	// クラスセット
	void SetAbilitySelectScreen(std::shared_ptr<AbilitySelectScreen> selectScreen);
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager);

private:
	void AbilitySelection();		// アビリティ選択処理
	bool IsSwitchPlayerByEnergy();	// エネルギーによるプレイヤー切り替えかどうかのチェック

protected:
	std::shared_ptr<AbilitySelectScreen> _abilitySelectScreen;	// 能力選択画面
	std::shared_ptr<PlayerManager> _playerManager;				// プレイヤーマネージャー
	
};

