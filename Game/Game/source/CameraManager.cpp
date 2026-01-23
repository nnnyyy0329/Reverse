#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"

CameraManager::CameraManager()
{
	// 初期タイプはゲームカメラ
	_eCameraType = CAMERA_TYPE::GAME_CAMERA;

	_gameCamera = nullptr;	// ゲームカメラ初期化
	_debugCamera = nullptr;	// デバッグカメラ初期化
}

CameraManager::~CameraManager()
{
	_gameCamera.reset();
	_debugCamera.reset();
}

bool CameraManager::Initialize()
{
	return true;
}

bool CameraManager::Terminate()
{
	return true;
}

bool CameraManager::Process()
{
	// カメラ切り替え処理
	SwitchCamera();

	// カメラ処理切り替え
	SwitchCameraProcess();

	return true;
}

bool CameraManager::Render()
{
	return true;
}

// カメラ切り替え
void CameraManager::SwitchCamera()
{
	if(_trg & KEY_INPUT_1)
	{
		// デバッグカメラへ切り替え
		if(_eCameraType == CAMERA_TYPE::GAME_CAMERA)
		{
			_eCameraType = CAMERA_TYPE::DEBUG_CAMERA;
		}
		// ゲームカメラへ切り替え
		else if(_eCameraType == CAMERA_TYPE::DEBUG_CAMERA)
		{
			_eCameraType = CAMERA_TYPE::GAME_CAMERA;
		}
	}
}

// カメラ処理切り替え
void CameraManager::SwitchCameraProcess()
{
	// カメラタイプによる処理分岐
	switch(_eCameraType)
	{
		// ゲームカメラ処理
		case CAMERA_TYPE::GAME_CAMERA:
		{
			if(_gameCamera)
			{
				_gameCamera->Process(_key, _trg, _lx, _ly, _rx, _ry, _analogMin, true);
			}

			break;
		}

		// デバッグカメラ処理
		case CAMERA_TYPE::DEBUG_CAMERA:
		{
			if(_debugCamera)
			{
				bool bIsPut = (_key & PAD_INPUT_2) != 0;// ボタン同時押し判定(B)

				_debugCamera->Process(_key, _trg, _lx, _ly, _rx, _ry, _analogMin, bIsPut);
			}

			break;
		}
	}
}
