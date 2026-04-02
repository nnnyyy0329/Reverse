/*****************************************
* file   ComponentManager.h
* brief  コンポーネント管理クラス
* author 成田 悠真
* date   2025/11/21
******************************************/

#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>


template<typename BaseType>

/// @brief コンポーネント管理クラス
class ComponentManager
{
public:

	/// @brief コンポーネントを追加
	///
	/// @param component 追加するコンポーネントのユニークポインタ
	void AddComponent(std::unique_ptr<BaseType> component)
	{
		// コンポーネントを追加
		_components.push_back(std::move(component));	
	}

	// 特定の型のコンポーネントを取得

	/// @brief 特定の型のコンポーネントを取得
	///
	/// @tparam T 取得するコンポーネントの型
	/// 
	/// @return 指定された型のコンポーネントのポインタ、見つからなかった場合は nullptr
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
				// 見つかった場合はポインタを返す
				return specific_component; 
			}
		}

		// 見つからなかった場合は nullptr を返す
		return nullptr; 
	}

	/// @brief 特定の型のコンポーネントを全て取得
	///
	/// @tparam T 取得するコンポーネントの型
	/// 
	/// @return 指定された型のコンポーネントのポインタを格納したベクター
	template<typename T>
	std::vector<T*> GetComponents()
	{
		// TがBaseTypeを継承していることを確認
		static_assert(std::is_base_of<BaseType, T>::value, "BaseType を T は継承していません");

		// 結果を格納するベクター
		std::vector<T*> result; 

		// コンポーネント配列を走査して特定の型のコンポーネントを探す
		for(auto& component : _components)
		{
			// 動的キャストを試みる
			if(auto specific_component = dynamic_cast<T*>(component.get())) // T型にキャストできた場合
			{
				// 見つかった場合は結果に追加
				result.push_back(specific_component); 
			}
		}

		// 結果のベクターを返す
		return result; 
	}

	/// @brief コンポーネントを全て取得
	///
	/// @return 全てのコンポーネントのユニークポインタを格納したベクター
	const std::vector<std::unique_ptr<BaseType>>& GetAllComponents() const
	{ 
		// 全てのコンポーネントを返す
		return _components;
	}

	// コンポーネントを削除

	/// @brief 特定の型のコンポーネントを削除
	///
	/// @tparam T 削除するコンポーネントの型
	/// 
	/// @return 削除に成功した場合は true、見つからなかった場合は false
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
		if(it != _components.end())
		{
			// 見つかった場合は削除
			_components.erase(it);	

			// 削除成功を返す
			return true;			
		}

		// 見つからなかった場合は削除失敗を返す
		return false; 
	}

	/// @brief 全てのコンポーネントをクリア
	void ClearComponents()
	{

		// 全てのコンポーネントを削除
		_components.clear();
	}

	/// @brief コンポーネント数を取得
	///
	/// @return 現在管理されているコンポーネントの数
	size_t GetComponentCount() const
	{
		// コンポーネントの数を返す
		return _components.size(); 
	}

private:

	// コンポーネントの配列
	std::vector<std::unique_ptr<BaseType>> _components; 
};

