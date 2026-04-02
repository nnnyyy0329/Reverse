/*****************************************
* file	 AbilityManager.h
* brief  能力管理クラス
* author 成田 悠真
* date   2025/12/27
******************************************/

#pragma once
#include "appframe.h"
#include "AbilityBase.h"

// 前方宣言
class PlayerBase;  

/// @brief 能力管理クラス
class AbilityManager
{
public:

	AbilityManager();
	virtual ~AbilityManager();


	/* 基本関数 */

	bool Initialize();
	bool Terminate();
	bool Process();

	// 能力管理用
	//bool SetAbility(std::unique_ptr<PlayerAbilityBase> ability);	// 能力追加
	//bool RemoveAbility();											// 能力削除
	//bool HasAbility() const;										// 能力チェック
	//PlayerAbilityBase* GetAbility() const;							// 能力取得
	//ABILITY_TYPE GetAbilityType() const;							// 能力タイプ取得

protected:

	std::unique_ptr<AbilityBase> _ability;	// 能力
	PlayerBase* _playerBase;				// 能力を持つプレイヤーへのポインタ
};
