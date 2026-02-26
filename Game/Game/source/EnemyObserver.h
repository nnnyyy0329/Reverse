#pragma once
#include "appframe.h"
#include "EnemyFactory.h"

// 敵の状態変化を受け取るインターフェース
class EnemyObserver
{
public:
	virtual ~EnemyObserver() = default;

	// 敵が倒されたときの通知関数
	virtual void OnEnemyDefeated(EnemyType enemyType) = 0;

	// オブザーバーの有効性チェック
	virtual bool IsValid() const { return true; }
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
	void AddObserver(EnemyObserver* observer);

	// オブザーバーの削除
	void RemoveObserver(EnemyObserver* observer);

	// 敵が倒されたときの通知
	void NotifyEnemyDefeated(EnemyType enemyType);

	// 全てのオブザーバーをクリア
	void ClearObservers(){ _observers.clear(); }

	// オブザーバーの数を取得
	size_t GetObserverCount() const { return _observers.size(); }

private:
	// シングルトンのため、コンストラクタ・デストラクタをprivateに
	EnemyNotifier() = default;
	~EnemyNotifier() = default;

	// コピー・ムーブ禁止
	EnemyNotifier(const EnemyNotifier&)				= delete;	// コピーコンストラクタ削除
	EnemyNotifier& operator=(const EnemyNotifier&)	= delete;	// コピー代入演算子削除
	EnemyNotifier(EnemyNotifier&&)					= delete;	// ムーブコンストラクタ削除
	EnemyNotifier& operator=(EnemyNotifier&&)		= delete;	// ムーブ代入演算子削除

	std::vector<EnemyObserver*>_observers;	// オブザーバーのリスト

};