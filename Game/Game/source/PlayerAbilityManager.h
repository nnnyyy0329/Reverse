// 担当 : 成田

#pragma once
#include "appframe.h"
#include "PlayerAbilityBase.h"

class PlayerBase;  // 前方宣言

class PlayerAbilityManager
{
public:
	PlayerAbilityManager();
	virtual ~PlayerAbilityManager();

	bool Initialize();
	bool Terminate();
	bool Process();

	// 能力管理用
	bool SetAbility(std::unique_ptr<PlayerAbilityBase> ability);	// 能力追加
	bool RemoveAbility();											// 能力削除
	bool HasAbility() const;										// 能力チェック
	PlayerAbilityBase* GetAbility() const;							// 能力取得
	ABILITY_TYPE GetAbilityType() const;							// 能力タイプ取得

protected:
	std::unique_ptr<PlayerAbilityBase> _ability;
	PlayerBase* _playerBase;
};
