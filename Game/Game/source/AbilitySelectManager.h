

#pragma once
#include "appframe.h"

// 前方宣言
class AbilitySelectScreen;
class PlayerManager;

/// @brief アビリティ選択管理クラス
class AbilitySelectManager
{
public:

	/* 基本関数 */

	bool Initialize();
	bool Terminate();
	bool Process();


	/* クラスセット */

	/// @brief 能力選択画面を設定
	///
	/// @param selectScreen 設定する能力選択画面のshared_ptr
	void SetAbilitySelectScreen(std::shared_ptr<AbilitySelectScreen> selectScreen);

	/// @brief プレイヤーマネージャーを設定
	///
	/// @param playerManager 設定するプレイヤーマネージャーのshared_ptr
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager);

private:

	/* アビリティ選択処理 */

	/// @brief アビリティ選択処理
	void AbilitySelection();		

	/// @brief エネルギーによるプレイヤー切り替えかどうかをチェックする関数
	/// 
	/// @return エネルギーによるプレイヤー切り替えならtrue、そうでないならfalse
	bool IsSwitchPlayerByEnergy();	

protected:

	// 能力選択画面
	std::shared_ptr<AbilitySelectScreen> _abilitySelectScreen;	

	// プレイヤーマネージャー
	std::shared_ptr<PlayerManager> _playerManager;				
	
};

