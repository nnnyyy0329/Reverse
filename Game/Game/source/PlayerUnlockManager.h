#pragma once
#include "appframe.h"
#include "EnemyFactory.h"

// プレイヤーの種類
enum class ABILITY_TYPE
{
	NONE,
	SURFACE_PLAYER,
	INTERIOR_PLAYER,
	BULLET_PLAYER,
	_EOT_,
};

// プレイヤー解放条件
struct UnlockCondition
{
	EnemyType requiredEnemyType;    // 必要な敵タイプ
	int requiredKillCount;          // 必要な撃破数
	ABILITY_TYPE unlockedAbility;   // 解放されるアビリティ
	std::string description;        // 解放条件の説明
};

// 解放イベントのコールバック
using UnlockCallback = std::function<void(ABILITY_TYPE)>;

// プレイヤーのアンロック管理クラス
class PlayerUnlockManager
{
public:
	PlayerUnlockManager();
	~PlayerUnlockManager();

	bool Initialize();
	bool Process();

	// 敵撃破通知
	void NotifyEnemyDefeated(EnemyType enemyType);

	// 解放条件を追加
	void AddUnlockCondition(EnemyType enemyType, int killCount, ABILITY_TYPE ability, const std::string& description = "");

	// 解放時のコールバック設定
	void SetUnlockCallback(UnlockCallback callback);
	
	// プレイヤーが解放されているかチェック
	bool IsAbilityUnlocked(ABILITY_TYPE ability) const;

	// 撃破数を取得
	int GetKillCount(EnemyType enemyType) const;

	// 全ての解放条件を取得
	const std::vector<UnlockCondition>& GetUnlockConditions() const;

	// デバッグ用の強制解放
	void ForceUnlock(ABILITY_TYPE ability);

	// デバッグ描画
	void DebugRender();

private:
	std::map<EnemyType, int> _killCounts;						// 敵タイプごとの撃破数
	std::set<ABILITY_TYPE> _unlockedAbilities;					// 解放済みアビリティ
	std::vector<UnlockCondition> _unlockConditions;				// 解放条件リスト
	std::string EnemyTypeToString(EnemyType type) const;		// 敵タイプを文字列に変換
	std::string AbilityTypeToString(ABILITY_TYPE type) const;	// アビリティタイプを文字列に変換

	// 解放時のコールバック
	UnlockCallback _unlockCallback;	

	// 内部処理
	void CheckUnlockConditions();				// 解放条件のチェック
	void UnlockAbility(ABILITY_TYPE ability);	// アビリティの解放処理

};

