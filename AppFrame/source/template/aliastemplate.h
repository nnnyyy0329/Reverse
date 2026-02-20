#pragma once
#include "container.h"

// エイリアステンプレート
namespace at
{
	template<typename C>
	using vspc = std::vector<std::shared_ptr<C>>;	// ベクターとスマートポインタとクラス

	template<typename C>
	using msc = std::map<std::string, C*>;			// マップとストリングとクラス

	template<typename C>
	using sp = std::shared_ptr<C>;					//スマートポインタとクラス

	template<typename C>
	using up = std::unipue_ptr<C>;					// ユニークポインタとクラス

	template<typename T>
	using mst = std::map<std::string, T>;			// マップとストリングとテンプレート(型)

	template<typename T>
	using ve = std::vector<T>;						// ベクターとテンプレート(型)

	template<typename C>
	using ls = std::list<C>;						// リストとクラス

	template<typename C>
	using wk = std::weak_ptr<C>;					// ウィークポインタとクラス
}
