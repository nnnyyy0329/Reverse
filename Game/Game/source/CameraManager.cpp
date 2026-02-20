#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"
#include "AimCamera.h"

CameraManager::CameraManager()
{
	_eCameraType = CAMERA_TYPE::GAME_CAMERA;	// 初期タイプはゲームカメラ
	_ePrevCameraType = CAMERA_TYPE::NONE;		// 前回のカメラタイプ初期化

	_gameCamera = nullptr;	// ゲームカメラ初期化
	_debugCamera = nullptr;	// デバッグカメラ初期化
	_aimCamera = nullptr;	// エイムカメラ初期化

	_bIsUseDebugCamera = false;	// デバッグカメラ使用フラグ初期化
}

CameraManager::~CameraManager()
{
	_gameCamera.reset();
	_debugCamera.reset();
	_aimCamera.reset();
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

	if(!IsAimMode())
	{
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

		// エイムカメラ処理
		case CAMERA_TYPE::AIM_CAMERA:
		{
			if(_aimCamera)
			{
				_aimCamera->Process(_key, _trg, _lx, _ly, _rx, _ry, _analogMin, true);
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

		// エイムカメラ設定
		case CAMERA_TYPE::AIM_CAMERA:
		{
			if(_aimCamera)
			{
				_aimCamera->SetUp();	// エイムカメラ設定更新
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

		// エイムカメラ描画
		case CAMERA_TYPE::AIM_CAMERA:
		{
			if(_aimCamera)
			{
				_aimCamera->Render();	// エイムカメラ描画
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

// 現在のカメラの水平角度を取得
float CameraManager::GetCurrentCameraAngleH() const
{
	// カメラタイプによる処理分岐
	switch(_eCameraType)
	{
		// ゲームカメラ
		case CAMERA_TYPE::GAME_CAMERA:
		{
			if(_gameCamera)
			{
				return _gameCamera->GetCameraAngleH();	// ゲームカメラの水平角度を返す
			}

			break;
		}

		// デバッグカメラ
		case CAMERA_TYPE::DEBUG_CAMERA:
		{
			if(_debugCamera)
			{
				//return _debugCamera->GetAngleH();	// デバッグカメラの水平角度を返す
			}

			break;
		}

		// エイムカメラ
		case CAMERA_TYPE::AIM_CAMERA:
		{
			if(_aimCamera)
			{
				return _aimCamera->GetAimAngleH(); // エイムカメラの水平角度を返す
			}

			break;
		}

		default:
		{
			break;
		}
	}

	return 0.0f; // デフォルト値を返す
}

// エイムモード開始
void CameraManager::StartAimMode()
{
	if(!_aimCamera){ return; }
	if(IsAimMode()){ return; }	// すでにエイムモード中だったら処理しない

	_ePrevCameraType = _eCameraType;		// 前回のカメラタイプを保存
	_eCameraType = CAMERA_TYPE::AIM_CAMERA;	// エイムカメラへ切り替え
	_aimCamera->StartAiming();				// エイムモード開始処理
}

// エイムモード終了
void CameraManager::EndAimMode()
{
	if(!_aimCamera){ return; }
	if(!IsAimMode()){ return; }	// エイムモード中でなかったら処理しない

	_eCameraType = _ePrevCameraType;	// 前回のカメラタイプに戻す
	_aimCamera->EndAiming();			// エイムモード終了処理
}

// エイムモード中か
bool CameraManager::IsAimMode()const
{
	return (_aimCamera->IsAiming());
}

// エイム方向取得
VECTOR CameraManager::GetAimDirection()const
{
	if(!_aimCamera && !_aimCamera->IsAiming()){ return VGet(0.0f, 0.0f, -1.0f); }

	return _aimCamera->GetAimDirection();
}

// プレイヤー設定
void CameraManager::SetPlayer(std::shared_ptr<PlayerBase> player)
{
	if(_gameCamera)
	{
		_gameCamera->SetTarget(player);	// ゲームカメラにターゲット設定
	}
	if(_aimCamera)
	{
		_aimCamera->SetTarget(player);	// エイムカメラにターゲット設定
	}
}