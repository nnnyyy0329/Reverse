/*****************************************
* file   EnemyObserver.h
* brief  敵の状態変化を受け取るクラス
* author 成田 悠真
* date   2026/02/24
******************************************/

#pragma once
#include "appframe.h"
#include "Enemy/EnemyFactory.h"

/// @brief 敵の状態変化を受け取るクラス
class EnemyObserver
{
public:

	virtual ~EnemyObserver() = default;

	/// @brief 敵が倒されたときに通知する純粋仮想関数
	///
	/// @param enemyType 倒された敵の種類
	virtual void OnEnemyDefeated(EnemyType enemyType) = 0;

	/// @brief オブザーバーの有効性をチェックする関数
	///
	/// @return オブザーバーが有効な場合はtrue、そうでない場合はfalse
	virtual bool IsValid() const { return true; }
};

/// @brief 敵の状態変化を通知するクラス
class EnemyNotifier
{
public:

	/// @brief シングルトンインスタンスの取得関数
	static EnemyNotifier* GetInstance()
	{
		static EnemyNotifier instance;	// 静的ローカル変数シングルトン
		return &instance;				// インスタンスのアドレスを返す
	}

	/// @brief オブザーバーの追加関数
	///
	/// @param observer 追加するオブザーバーのポインタ
	void AddObserver(EnemyObserver* observer);

	/// @brief オブザーバーの削除関数
	///
	/// @param observer 削除するオブザーバーのポインタ
	void RemoveObserver(EnemyObserver* observer);

	/// @brief 敵が倒されたときの通知関数
	///
	/// @param enemyType 倒された敵の種類
	void NotifyEnemyDefeated(EnemyType enemyType);

	/// @brief 全てのオブザーバーをクリアする関数
	void ClearObservers(){ _observers.clear(); }

	/// @brief オブザーバーの数を取得する関数
	///
	/// @return オブザーバーの数
	size_t GetObserverCount() const { return _observers.size(); }

private:

	// シングルトンのため、コンストラクタ・デストラクタをprivateにいれる
	EnemyNotifier()				= default;
	virtual ~EnemyNotifier()	= default;

	/* コピー・ムーブ禁止 */
	
	/// @brief コピーコンストラクタ削除関数
	EnemyNotifier(const EnemyNotifier&)				= delete;	
	
	/// @brief コピー代入演算子削除関数
	EnemyNotifier& operator=(const EnemyNotifier&)	= delete;	
	
	/// @brief ムーブコンストラクタ削除関数
	EnemyNotifier(EnemyNotifier&&)					= delete;	
	
	/// @brief ムーブ代入演算子削除関数
	EnemyNotifier& operator=(EnemyNotifier&&)		= delete;	

	std::vector<EnemyObserver*>_observers;	// オブザーバーのリスト

};