#pragma once
#include <string>

// メニュー項目用ベースクラス
class MenuItemBase {
public:
	MenuItemBase(void* param, std::string text) : _param(param), _text(text) {}
	virtual ~MenuItemBase() {};

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() { return 0; }

	void* _param;
	std::string _text;
};
