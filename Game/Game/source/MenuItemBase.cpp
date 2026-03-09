#include "MenuItemBase.h"
#include "ModeGame.h"
#include "ModeMenu.h"
#include "CameraManager.h"

int MenuItemViewDebugInfo::Selected()
{
	ModeGame* mdGame = static_cast<ModeGame*>(_param);
	mdGame->SetDebugViewDebugInfo(!mdGame->GetDebugViewDebugInfo());
	return 0;
}



int MenuItemViewCollision::Selected()
{
	ModeGame* mdGame = static_cast<ModeGame*>(_param);
	mdGame->SetDebugViewCollision(!mdGame->GetDebugViewCollision());
	return 0;
}



int MenuItemUseCollision::Selected()
{
	ModeGame* mdGame = static_cast<ModeGame*>(_param);
	mdGame->SetDebugUseCollision(!mdGame->GetDebugUseCollision());
	return 0;
}
 


// MenuDebugCamera
MenuDebugCamera::MenuDebugCamera(void* param, std::string text)
	: MenuItemBase(param, text)
	, _cameraManager(nullptr)
{
}

void MenuDebugCamera::SetCameraManagerMenu(std::shared_ptr<CameraManager> cameraManager)
{
	_cameraManager = cameraManager;
}

int MenuDebugCamera::Selected()
{
	if (!_cameraManager) { return 0; }

	// デバッグカメラを有効化
	_cameraManager->SetIsUseDebugCamera(true);

	// メニュー側に通知
	auto* modeMenu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
	if (modeMenu)
	{
		modeMenu->SetUseDebugCamera(true);
	}

	return 0;
}