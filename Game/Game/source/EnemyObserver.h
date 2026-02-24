#pragma once
#include "EnemyFactory.h"

// 敵の状態変化を受け取るインターフェース
class EnemyObserver
{
public:
	virtual ~EnemyObserver() = default;

	// 敵が倒されたときの通知関数
	virtual void OnEnemyDefeated(EnemyType enemyType) = 0;
};

// 敵の状態変化を通知するクラス
class EnemyNotifier
{
public:
	// シングルトンインスタンスの取得
	static EnemyNotifier* GetInstance()
	{
		static EnemyNotifier instance;
		return &instance;
	}

	// オブザーバーの追加
	void AddObserver(EnemyObserver* observer)
	{
		// オブザーバーをリストに追加
		_observers.push_back(observer);
	}

	// オブザーバーの削除
	void RemoveObserver(EnemyObserver* observer)
	{
		// オブザーバーをリストから削除
		_observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
	}

	// 敵が倒されたときの通知
	void NotifyEnemyDefeated(EnemyType enemyType)
	{
		// 登録された全てのオブザーバーに通知
		for(auto* observer : _observers)
		{
			// 敵が倒されたことを通知
			observer->OnEnemyDefeated(enemyType);
		}
	}

private:
	std::vector<EnemyObserver*>_observers;	// オブザーバーのリスト
};