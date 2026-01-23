#pragma once
#include "appframe.h"
#include "ModeGame.h"

// メニュー項目用ベースクラス
class MenuItemBase 
{
public:
	MenuItemBase(void* param, std::string text) : _param(param), _text(text) {}
	virtual ~MenuItemBase() {};

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() { return 0; }

	void* _param;
	std::string _text;
};

class MenuItemViewCollision : public MenuItemBase
{
public:
	MenuItemViewCollision(void* param, std::string text) : MenuItemBase(param, text) {}

	virtual int Selected()
	{
		ModeGame* mdGame = static_cast<ModeGame*>(_param);
		mdGame->SetDebugViewCollision(!mdGame->GetDebugViewCollision());
		return 1;
	}
};

class MenuItemUseCollision : public MenuItemBase
{
public:
	MenuItemUseCollision(void* param, std::string text) : MenuItemBase(param, text) {}
	virtual int Selected()
	{
		ModeGame* mdGame = static_cast<ModeGame*>(_param);
		mdGame->SetUseCollision(!mdGame->GetUseCollision());
		return 1;
	}
};
