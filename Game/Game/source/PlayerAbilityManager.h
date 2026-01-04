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
	bool AddAbility(std::unique_ptr<PlayerAbilityBase> ability);	// 能力追加
	bool RemoveAbility(ABILITY_TYPE type);							// 能力削除
	bool HasAbility(ABILITY_TYPE type) const;						// 能力チェック
	PlayerAbilityBase* GetAbility(ABILITY_TYPE type) const;			// 能力取得

protected:
	std::unordered_map<ABILITY_TYPE, std::unique_ptr<PlayerAbilityBase>> _abilities;
	PlayerBase* _playerBase;
};
