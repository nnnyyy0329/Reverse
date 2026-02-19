#include "MenuItemBase.h"
#include "ModeGame.h"
#include "ModeMenu.h"
#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"


// ...（既存のコードはそのまま）...

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
	, __cameraManager(nullptr)
	, __debugCamera(nullptr)
	, __gameCamera(nullptr)
{
}

void MenuDebugCamera::SetCameraManagerMenu(std::shared_ptr<CameraManager> cameraManager)
{
	__cameraManager = cameraManager;
}

void MenuDebugCamera::SetGameCameraMenu(std::shared_ptr<GameCamera> gameCamera)
{
	__gameCamera = gameCamera;
}

void MenuDebugCamera::SetDebugCameraMenu(std::shared_ptr<DebugCamera> debugCamera)
{
	__debugCamera = debugCamera;
}

int MenuDebugCamera::Selected()
{
	// カメラ情報の設定
	if (__debugCamera && __gameCamera && __cameraManager)
	{
		// ゲームカメラの現在位置と注視点をデバッグカメラに設定
		__debugCamera->SetInfo(__gameCamera->GetVPos(), __gameCamera->GetVTarget());

		// デバッグカメラを有効化
		__cameraManager->SetIsUseDebugCamera(true);

		// ModeMenuにもデバッグカメラ使用を通知
		auto* modeMenu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
		if (modeMenu)
		{
			modeMenu->SetUseDebugCamera(true);
		}
	}

	return 0;
}

//MenuItemVolume::MenuItemVolume(void* param, const std::string& soundName, int initVolume)
//	: MenuItemBase(param, "")
//	, _soundName(soundName)
//	, _volume(0)
//{
//	if(_soundName.empty())
//	{
//		// マスター音量
//		_volume = SoundServer::GetInstance()->GetMasterVolume();
//	}
//	else
//	{
//		// C++14 対応：std::clamp を使わずに範囲制限
//		_volume = std::max(0, std::min(initVolume, 255));
//	}
//	UpdateText();
//}
//
//void MenuItemVolume::Increase()
//{
//	_volume = std::min(255, _volume + 16);
//	if(_soundName.empty())
//	{
//		SoundServer::GetInstance()->SetMasterVolume(_volume);
//	}
//	else
//	{
//		SoundServer::GetInstance()->SetVolume(_soundName, _volume);
//	}
//	UpdateText();
//}
//
//void MenuItemVolume::Decrease()
//{
//	_volume = std::max(0, _volume - 16);
//	if(_soundName.empty())
//	{
//		SoundServer::GetInstance()->SetMasterVolume(_volume);
//	}
//	else
//	{
//		SoundServer::GetInstance()->SetVolume(_soundName, _volume);
//	}
//	UpdateText();
//}
//
//void MenuItemVolume::UpdateText()
//{
//	int percent = (_volume * 100) / 255;
//	if(_soundName.empty())
//	{
//		_text = "Master Volume: " + std::to_string(percent) + "%";
//	}
//	else
//	{
//		_text = _soundName + ": " + std::to_string(percent) + "%";
//	}
//}