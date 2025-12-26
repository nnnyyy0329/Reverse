// 担当 : 成田

#include "PlayerAbilityManager.h"

PlayerAbilityManager::PlayerAbilityManager()
{

}

PlayerAbilityManager::~PlayerAbilityManager()
{
	// すべての能力を削除
	_abilities.clear();
}

bool PlayerAbilityManager::Initialize()
{
	return true;
}

bool PlayerAbilityManager::Terminate()
{
	return true;
}

bool PlayerAbilityManager::Process()
{
	return true;
}

// 能力追加関数
bool PlayerAbilityManager::AddAbility(std::unique_ptr<PlayerAbilityBase> ability)
{
	if(!ability){ return false; }

	ABILITY_TYPE type = ability->GetAbilityType();

	// 既に同じタイプの能力が存在するかチェック
	if(HasAbility(type)){ return false; }

	// 能力マップに追加
	_abilities[type] = std::move(ability);

	return true;
}

// 能力削除
bool PlayerAbilityManager::RemoveAbility(ABILITY_TYPE type)
{
	auto ability = _abilities.find(type);
	if(ability == _abilities.end()){ return false; }	// 存在しない

	// マップから削除
	_abilities.erase(ability);

	return true;
}

// 能力チェック関数
bool PlayerAbilityManager::HasAbility(ABILITY_TYPE type) const
{
	return _abilities.find(type) != _abilities.end();
}

// 能力取得
PlayerAbilityBase* PlayerAbilityManager::GetAbility(ABILITY_TYPE type) const
{
	auto ability = _abilities.find(type);
	if(ability != _abilities.end())
	{
		return ability->second.get();	// アビリティ取得
	}
	return nullptr;
}