#pragma once
#include "appframe.h"
#include "MenuItemBase.h"

//class DebugCamera;
class CameraManager;

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

	// カメラ設定
	void SetCameraManager(std::shared_ptr<CameraManager> camera) { _cameraManager = camera; }

	// メニュー中にデバッグカメラを使っているか
	bool IsUseDebugCamera() const { return _bUseDebugCamera; }
	void SetUseDebugCamera(bool useCamera){ _bUseDebugCamera = useCamera; }

protected:
	std::vector<std::unique_ptr<MenuItemBase>> _menuItems;

	int _curPos;
	int _curAnimCnt;

	std::shared_ptr<CameraManager> _cameraManager;
	bool _bUseDebugCamera;
};

