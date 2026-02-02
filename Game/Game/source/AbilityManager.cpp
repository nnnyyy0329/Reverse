// 担当 : 成田

#include "AbilityManager.h"

AbilityManager::AbilityManager()
{
	_ability = nullptr;
	_playerBase = nullptr;
}

AbilityManager::~AbilityManager()
{
	// すべての能力を削除
	_ability.reset();
}

bool AbilityManager::Initialize()
{
	return true;
}

bool AbilityManager::Terminate()
{
	return true;
}

bool AbilityManager::Process()
{
	return true;
}

//// 能力設定関数
//bool AbilityManager::SetAbility(std::unique_ptr<PlayerAbilityBase> ability)
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
//bool AbilityManager::RemoveAbility()
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
//bool AbilityManager::HasAbility() const
//{
//	return _ability != nullptr;
//}
//
//// 能力取得
//PlayerAbilityBase* AbilityManager::GetAbility() const
//{
//	return _ability.get();
//}
//
//// 能力タイプ取得
//ABILITY_TYPE AbilityManager::GetAbilityType() const
//{
//	if(_ability)
//	{
//		return _ability->GetAbilityType();
//	}
//
//	return ABILITY_TYPE::NONE;
//}