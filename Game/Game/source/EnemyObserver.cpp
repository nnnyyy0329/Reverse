#include "EnemyObserver.h"

// オブザーバーの追加
void EnemyNotifier::AddObserver(EnemyObserver* observer)
{
	if(!observer) return; // nullptr チェック

	// 重複チェック
	auto it = std::find(_observers.begin(), _observers.end(), observer);
	if(it == _observers.end())
	{
		_observers.push_back(observer);
	}
}

// オブザーバーの削除
void EnemyNotifier::RemoveObserver(EnemyObserver* observer)
{
	if(!observer) return; // nullptr チェック

	// std::removeとeraseを組み合わせて、指定されたオブザーバーをリストから削除
	_observers.erase(std::remove(_observers.begin(), _observers.end(), observer), _observers.end());
}

// 敵が倒されたときの通知
void EnemyNotifier::NotifyEnemyDefeated(EnemyType enemyType)
{
	// オブザーバーのコピーを作成
	auto observersCopy = _observers;	// コピーを作成

	// コピーされたリストを使用して通知
	for(auto* observer : observersCopy)
	{
		// オブザーバーが有効かどうかをチェック
		if(observer && observer->IsValid())
		{
			// 例外が発生する可能性のあるコードをtryブロックで囲む
			try // 例外が発生する可能性のあるコード
			{
				// ここでオブザーバーに通知を送る
				observer->OnEnemyDefeated(enemyType);
			}
			// 例外が発生した場合は、オブザーバーを削除する
			catch(...) // すべての例外をキャッチ
			{
				// 例外が発生した場合、そのオブザーバーを削除
				RemoveObserver(observer);
			}
		}
	}
}
