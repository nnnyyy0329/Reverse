#pragma once
#include "appframe.h"

// 前方宣言
class ModeGame;
class CameraManager;

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

// デバッグ情報表示、非表示
class MenuItemViewDebugInfo : public MenuItemBase
{
public:
	MenuItemViewDebugInfo(void* param, std::string text) : MenuItemBase(param, text) {}
	virtual int Selected();
};

// コリジョン表示、非表示
class MenuItemViewCollision : public MenuItemBase
{
public:
	MenuItemViewCollision(void* param, std::string text) : MenuItemBase(param, text) {}
	virtual int Selected();
};

// コリジョン有効、無効
class MenuItemUseCollision : public MenuItemBase
{
public:
	MenuItemUseCollision(void* param, std::string text) : MenuItemBase(param, text) {}
	virtual int Selected();
};

// デバッグカメラ切り替え
class MenuDebugCamera : public MenuItemBase
{
public:
	MenuDebugCamera(void* param, std::string text);

	void SetCameraManagerMenu(std::shared_ptr<CameraManager> cameraManager);

	virtual int Selected();

protected:
	std::shared_ptr<CameraManager> _cameraManager;

};
