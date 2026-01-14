#pragma once
#include "appframe.h"
#include "MenuItemBase.h"

class DebugCamera;

class ModeMenu : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

	void AddMenuItem(MenuItemBase* item);
	void ClearMenuItems();

	// modegame側からデバッグカメラを渡す
	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { _debugCamera = debugCamera; }

	// メニュー中にデバッグカメラを使っているか
	bool IsUseDebugCamera() const { return _bUseDebugCamera; }

protected:
	std::vector<std::unique_ptr<MenuItemBase>> _menuItems;

	int _curPos;
	int _curAnimCnt;

	std::shared_ptr<DebugCamera> _debugCamera;
	bool _bUseDebugCamera;
};

