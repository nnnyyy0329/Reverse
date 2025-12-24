#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>

// コンポーネント管理のためのユーティリティクラス
template<typename BaseType>

class ComponentManager
{
public:
	// コンポーネントを追加
	void AddComponent(std::unique_ptr<BaseType> component)
	{
		_components.push_back(std::move(component));	// コンポーネントを追加
	}

	// 特定の型のコンポーネントを取得
	template<typename T>
	T* GetComponent()
	{
		// TがBaseTypeを継承していることを確認
		static_assert(std::is_base_of<BaseType, T>::value, "BaseType を T は継承していません");

		// コンポーネント配列を走査して特定の型のコンポーネントを探す
		for(auto& component : _components)
		{
			// 動的キャストを試みる
			if(auto specific_component = dynamic_cast<T*>(component.get()))	// T型にキャストできた場合
			{
				return specific_component; // 見つかった場合はポインタを返す
			}
		}

		return nullptr; // 見つからなかった場合は nullptr を返す
	}

	// 特定の型のコンポーネントを全て取得
	template<typename T>
	std::vector<T*> GetComponents()
	{
		// TがBaseTypeを継承していることを確認
		static_assert(std::is_base_of<BaseType, T>::value, "BaseType を T は継承していません");

		std::vector<T*> result; // 結果を格納するベクター

		// コンポーネント配列を走査して特定の型のコンポーネントを探す
		for(auto& component : _components)
		{
			// 動的キャストを試みる
			if(auto specific_component = dynamic_cast<T*>(component.get())) // T型にキャストできた場合
			{
				result.push_back(specific_component); // 見つかった場合は結果に追加
			}
		}
		return result; // 結果のベクターを返す
	}

	// コンポーネントを全て取得
	const std::vector<std::unique_ptr<BaseType>>& GetAllComponents() const
	{ 
		return _components;
	}

	// コンポーネントを削除
	template<typename T>
	bool RemoveComponent()
	{
		// TがBaseTypeを継承していることを確認
		static_assert(std::is_base_of<BaseType, T>::value, "BaseType を T は継承していません");

		// 指定された型のコンポーネントを検索
		auto it = std::find_if(_components.begin(), _components.end(), [](const auto& component)
		{
			// 動的キャストを試みる
			return dynamic_cast<T*>(component.get()) != nullptr;	// T型にキャストできた場合
		});

		// コンポーネントが見つかった場合
		if(it != _components.end())	// 見つかった場合
		{
			_components.erase(it);	// 見つかった場合は削除
			return true;			// 削除成功を返す
		}

		// 見つからなかった場合は削除失敗を返す
		return false; 
	}

	// 全てのコンポーネントをクリア
	void ClearComponents()
	{ 
		_components.clear();
	}

	// コンポーネント数を取得
	size_t GetComponentCount() const
	{
		return _components.size(); 
	}

private:
	std::vector<std::unique_ptr<BaseType>> _components; // コンポーネントの配列
};

