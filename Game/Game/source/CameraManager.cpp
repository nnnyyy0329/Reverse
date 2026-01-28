#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"

CameraManager::CameraManager()
{
	_eCameraType = CAMERA_TYPE::GAME_CAMERA;	// 初期タイプはゲームカメラ
	_bIsUseDebugCamera = false;					// デバッグカメラ使用フラグ初期化

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

// カメラ切り替え
void CameraManager::SwitchCamera()
{
	if(_trg & PAD_INPUT_4)
	{
		// デバッグカメラ使用フラグ切り替え
		//_bIsUseDebugCamera = !_bIsUseDebugCamera;
	}

	// カメラタイプ切り替え
	if(_bIsUseDebugCamera)
	{
		// デバッグカメラへ切り替え
		_eCameraType = CAMERA_TYPE::DEBUG_CAMERA;
	}
	else 
	{
		// ゲームカメラへ切り替え
		_eCameraType = CAMERA_TYPE::GAME_CAMERA;
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
				_gameCamera->Process(_key, _trg, _lx, _ly, _rx, _ry, _analogMin, false);
			}

			break;
		}

		// デバッグカメラ処理
		case CAMERA_TYPE::DEBUG_CAMERA:
		{
			if(_debugCamera)
			{
				bool isInput = false;

				// 押している間は入力を有効にする
				if(_key & PAD_INPUT_2){ isInput = true; }

				_debugCamera->Process(_key, _trg, _lx, _ly, _rx, _ry, _analogMin, isInput);
			}

			break;
		}
	}
}

// カメラ設定切り替え
void CameraManager::SwitchCameraSetUp()
{
	// カメラタイプによる処理分岐
	switch(_eCameraType)
	{
		// ゲームカメラ設定
		case CAMERA_TYPE::GAME_CAMERA:
		{
			if(_gameCamera)
			{
				_gameCamera->SetUp();	// ゲームカメラ設定更新
			}
			break;
		}
		// デバッグカメラ設定
		case CAMERA_TYPE::DEBUG_CAMERA:
		{
			if(_debugCamera)
			{
				_debugCamera->SetUp();	// デバッグカメラ設定更新
			}
			break;
		}
	}
}

// カメラ描画切り替え
void CameraManager::SwitchCameraRender()
{
	// カメラタイプによる処理分岐
	switch(_eCameraType)
	{
		// ゲームカメラ描画
		case CAMERA_TYPE::GAME_CAMERA:
		{
			if(_gameCamera)
			{
				_gameCamera->Render();	// ゲームカメラ描画
			}
			break;
		}
		// デバッグカメラ描画
		case CAMERA_TYPE::DEBUG_CAMERA:
		{
			if(_debugCamera)
			{
				_debugCamera->Render();	// デバッグカメラ描画
			}
			break;
		}
	}
}

// カメラデバッグ描画切り替え
void CameraManager::SwitchCameraDebugRender()
{
	//// カメラタイプによる処理分岐
	//switch(_eCameraType)
	//{
	//	// ゲームカメラデバッグ描画
	//	case CAMERA_TYPE::GAME_CAMERA:
	//	{
	//		if(_gameCamera)
	//		{
	//			_gameCamera->DebugRender();	// ゲームカメラデバッグ描画
	//		}
	//		break;
	//	}
	//	// デバッグカメラデバッグ描画
	//	case CAMERA_TYPE::DEBUG_CAMERA:
	//	{
	//		if(_debugCamera)
	//		{
	//			_debugCamera->DebugRender();	// デバッグカメラデバッグ描画
	//		}
	//		break;
	//	}
	//}

	int x = 400;
	int y = 90;
	// デバッグカメラ使用フラグ表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "Use Debug Camera: %s", _bIsUseDebugCamera ? "True" : "False");
}