#include "MenuItemBase.h"
#include "ModeGame.h"
#include "ModeMenu.h"
#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"
#include <Server/SoundServer.h> // 音量制御のため追加


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

// MenuItemNumber 実装（音量対応版）
MenuItemNumber::MenuItemNumber(void* param, const std::string& name, int initValue, int minValue, int maxValue, bool linkToVolume)
	: MenuItemBase(param, "")
	, _name(name)
	, _value(initValue)
	, _minValue(minValue)
	, _maxValue(maxValue)
	, _linkToVolume(linkToVolume)
{
	// 範囲チェック
	if (_value < _minValue) _value = _minValue;
	if (_value > _maxValue) _value = _maxValue;
	UpdateText();
	
	// 初期音量設定
	if (_linkToVolume) {
		UpdateVolume();
	}
}

void MenuItemNumber::Increase()
{
	_value++;
	if (_value > _maxValue) _value = _maxValue;
	UpdateText();
	
	// 音量連動
	if (_linkToVolume) {
		UpdateVolume();
	}
}

void MenuItemNumber::Decrease()
{
	_value--;
	if (_value < _minValue) _value = _minValue;
	UpdateText();
	
	// 音量連動
	if (_linkToVolume) {
		UpdateVolume();
	}
}

void MenuItemNumber::UpdateText()
{
	if (_linkToVolume) {
		_text = _name + ": " + std::to_string(_value) + " (Volume)";
	} else {
		_text = _name + ": " + std::to_string(_value);
	}
}

void MenuItemNumber::UpdateVolume()
{
	// 0-100 の値を 0-255 の音量に変換してマスター音量に設定
	int volume = (_value * 255) / 100;
	SoundServer::GetInstance()->SetMasterVolume(volume);
}