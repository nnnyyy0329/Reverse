#include "PlayerUnlockManager.h"

PlayerUnlockManager::PlayerUnlockManager()
{
	// デフォルトで表プレイヤーは解放
	_unlockedAbilities.insert(ABILITY_TYPE::SURFACE_PLAYER);
}

PlayerUnlockManager::~PlayerUnlockManager()
{

}

bool PlayerUnlockManager::Initialize()
{
	// デフォルト解放条件の追加
	AddUnlockCondition(EnemyType::TANK, 1, ABILITY_TYPE::INTERIOR_PLAYER, "パワー型の敵を1体撃破でPower能力解放");
	AddUnlockCondition(EnemyType::RANGED, 1, ABILITY_TYPE::BULLET_PLAYER, "射撃型の敵を1体撃破でBraster能力解放");

	return true;
}

bool PlayerUnlockManager::Process()
{
	// 解放条件のチェック
	CheckUnlockConditions();

	return true;
}

void PlayerUnlockManager::NotifyEnemyDefeated(EnemyType enemyType)
{
	_killCounts[enemyType]++;	// 撃破数をカウントアップ
}

void PlayerUnlockManager::AddUnlockCondition(EnemyType enemyType, int killCount, ABILITY_TYPE ability, const std::string& description)
{
	UnlockCondition condition;

	condition.requiredEnemyType = enemyType;	// 必要な敵タイプを設定
	condition.requiredKillCount = killCount;	// 必要な撃破数を設定
	condition.unlockedAbility = ability;		// 解放されるアビリティを設定
	condition.description = description;		// 解放条件の説明を設定

	_unlockConditions.push_back(condition);	// 解放条件リストに追加
}

void PlayerUnlockManager::SetUnlockCallback(UnlockCallback callback)
{
	_unlockCallback = callback;	// コールバックを設定
}

bool PlayerUnlockManager::IsAbilityUnlocked(ABILITY_TYPE ability) const
{
	return _unlockedAbilities.find(ability) != _unlockedAbilities.end();	// 解放されているかチェック
}

int PlayerUnlockManager::GetKillCount(EnemyType enemyType) const
{
	// 敵タイプに対応する撃破数を取得
	auto it = _killCounts.find(enemyType);

	// 撃破数が記録されている場合はその数を返す
	if(it != _killCounts.end())
	{
		return it->second;	// 撃破数を返す
	}
	// 撃破数が記録されていない場合は0を返す
	else
	{
		return 0;	// 撃破数がない場合は0を返す
	}
}

const std::vector<UnlockCondition>& PlayerUnlockManager::GetUnlockConditions() const
{
	return _unlockConditions;	// 解放条件リストを返す
}

void PlayerUnlockManager::CheckUnlockConditions()
{
	for(auto& condition : _unlockConditions)
	{
		// すでに解放されている場合はスキップ
		if(IsAbilityUnlocked(condition.unlockedAbility)) // 解放済み解放されているかチェック
		{
			continue;	// 解放済みならスキップ
		}

		int currentKillCount = GetKillCount(condition.requiredEnemyType);	// 現在の撃破数を取得

		// 条件を満たしているかチェック
		if(currentKillCount >= condition.requiredKillCount)	// キル数が必要数以上なら
		{
			// 条件を満たしているのでアビリティを解放
			UnlockAbility(condition.unlockedAbility);
		}
	}
}

void PlayerUnlockManager::UnlockAbility(ABILITY_TYPE ability)
{
	_unlockedAbilities.insert(ability);	// アビリティを解放済みにする

	// コールバックが設定されている場合は呼び出す
	if(_unlockCallback)
	{
		// 解放されたアビリティをコールバックに渡す
		_unlockCallback(ability);	
	}
}

void PlayerUnlockManager::ForceUnlock(ABILITY_TYPE ability)
{
	// アビリティを強制的に解放
	UnlockAbility(ability);	
}

void PlayerUnlockManager::DebugRender()
{

}

std::string PlayerUnlockManager::EnemyTypeToString(EnemyType type) const
{
	switch(type)
	{
		case EnemyType::MELEE:	return "Melee";		// 近接
		case EnemyType::RANGED: return "Ranged";	// 遠距離
		case EnemyType::TANK:	return "Tank";		// タンク
		default: return "Unknown";
	}
}

std::string PlayerUnlockManager::AbilityTypeToString(ABILITY_TYPE type) const
{
	switch(type)
	{
		case ABILITY_TYPE::NONE:			return "None";				// なし
		case ABILITY_TYPE::SURFACE_PLAYER:	return "Surface Player";	// デフォルトで解放されている表プレイヤー
		case ABILITY_TYPE::INTERIOR_PLAYER: return "Interior Player";	// タンク撃破で解放される裏プレイヤー
		case ABILITY_TYPE::BULLET_PLAYER:	return "Bullet Player";		// 射撃型撃破で解放される弾プレイヤー
		default: return "Unknown";
	}
}

