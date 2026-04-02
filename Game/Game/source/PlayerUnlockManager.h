/*****************************************
* file   PlayerUnlockManager.h
* brief  プレイヤーのアンロック管理クラス
* author 成田 悠真
* date   2026/02/24
******************************************/

#pragma once
#include "appframe.h"
#include "EnemyObserver.h"
#include "AbilityBase.h"

/// @brief 解放イベントのコールバック関数の型定義
using UnlockCallback = std::function<void(ABILITY_TYPE)>;

/// @brief プレイヤー解放条件構造体
struct UnlockCondition
{
	EnemyType requiredEnemyType;    // 必要な敵タイプ
	int requiredKillCount;          // 必要な撃破数
	ABILITY_TYPE unlockedAbility;   // 解放されるアビリティ
	std::string description;        // 解放条件の説明
};

/// @brief プレイヤーのアンロック管理クラス
class PlayerUnlockManager
{
public:

	PlayerUnlockManager();
	virtual ~PlayerUnlockManager();


	/* 基本関数 */

	bool Initialize();
	bool Process();

	
	/* プレイヤー解放管理関数 */

	/// @brief 敵が倒されたときの通知関数
	///
	/// @param enemyType 倒された敵の種類
	void NotifyEnemyDefeated(EnemyType enemyType);

	/// @brief 解放条件を追加する関数
	///
	/// @param enemyType 必要な敵タイプ
	/// @param killCount 必要な撃破数
	/// @param ability 解放されるアビリティ
	/// @param description 解放条件の説明
	void AddUnlockCondition(EnemyType enemyType, int killCount, ABILITY_TYPE ability, const std::string& description = "");

	/// @brief 解放イベントのコールバック関数を設定する関数
	///
	/// @param callback 設定するコールバック関数
	void SetUnlockCallback(UnlockCallback callback);
	
	/// @brief 指定されたアビリティが解放されているかチェックする関数
	///
	/// @param ability チェックするアビリティの種類
	/// 
	/// @return 指定されたアビリティが解放されている場合はtrue、そうでない場合はfalse
	bool IsAbilityUnlocked(ABILITY_TYPE ability) const;

	/// @brief 指定された敵タイプの撃破数を取得する関数
	///
	/// @param enemyType 撃破数を取得する敵の種類
	/// 
	/// @return 指定された敵タイプの撃破数
	int GetKillCount(EnemyType enemyType) const;

	/// @brief 全ての解放条件を取得する関数
	///
	/// @return 全ての解放条件のリスト
	const std::vector<UnlockCondition>& GetUnlockConditions() const;


	/* デバッグ処理 */

	/// @brief デバッグ描画関数
	void DebugRender();

	/// @brief 指定されたアビリティを強制的に解放する関数
	///
	/// @param ability 強制的に解放するアビリティの種類
	void ForceUnlock(ABILITY_TYPE ability);

private:

	/* 内部処理 */

	/// @brief 解放条件をチェックして条件を満たすアビリティを解放する関数
	void CheckUnlockConditions();

	/// @brief アビリティを解放する関数
	///
	/// @param ability 解放するアビリティの種類
	void UnlockAbility(ABILITY_TYPE ability);

	/// @brief 敵タイプを文字列に変換する関数
	///
	/// @param type 変換する敵タイプ
	std::string EnemyTypeToString(EnemyType type) const;		
		
	/// @brief アビリティタイプを文字列に変換する関数
	///
	/// @param type 変換するアビリティタイプ
	std::string AbilityTypeToString(ABILITY_TYPE type) const;	

	std::map<EnemyType, int> _killCounts;			// 敵タイプごとの撃破数管理マップ
	std::set<ABILITY_TYPE> _unlockedAbilities;		// 解放済みアビリティ
	std::vector<UnlockCondition> _unlockConditions;	// 解放条件リスト

	// 解放時のコールバック
	UnlockCallback _unlockCallback;	

};

