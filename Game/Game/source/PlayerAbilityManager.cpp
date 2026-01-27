// 担当 : 成田

#include "PlayerAbilityManager.h"

PlayerAbilityManager::PlayerAbilityManager()
{
	_ability = nullptr;
	_playerBase = nullptr;
}

PlayerAbilityManager::~PlayerAbilityManager()
{
	// すべての能力を削除
	_ability.reset();
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

//// 能力設定関数
//bool PlayerAbilityManager::SetAbility(std::unique_ptr<PlayerAbilityBase> ability)
//{
//	if(!ability){ return false; }
//
//	// 既存のアビリティがあれば自動的に削除され、新しいアビリティに置換
//	_ability = std::move(ability);
//
//	return true;
//}
//
//// 能力削除
//bool PlayerAbilityManager::RemoveAbility()
//{
//	if(!_ability){ return false; }
//
//	// アビリティを削除
//	_ability.reset();
//
//	return true;
//}
//
//// 能力チェック関数
//bool PlayerAbilityManager::HasAbility() const
//{
//	return _ability != nullptr;
//}
//
//// 能力取得
//PlayerAbilityBase* PlayerAbilityManager::GetAbility() const
//{
//	return _ability.get();
//}
//
//// 能力タイプ取得
//ABILITY_TYPE PlayerAbilityManager::GetAbilityType() const
//{
//	if(_ability)
//	{
//		return _ability->GetAbilityType();
//	}
//
//	return ABILITY_TYPE::NONE;
//}