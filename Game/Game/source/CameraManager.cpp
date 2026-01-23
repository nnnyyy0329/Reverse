#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"

CameraManager::CameraManager()
{

}

CameraManager::~CameraManager()
{

}

bool CameraManager::Initialize()
{
	// ゲームカメラの生成と初期化
	_gameCamera = std::make_shared<GameCamera>();
	_gameCamera->Initialize();
	// デバッグカメラの生成と初期化
	_debugCamera =  std::make_shared<DebugCamera>();
	_debugCamera->Initialize();
	// 初期アクティブカメラはゲームカメラ
	_activeCameraType = CameraType::GAME;
	return true;
}

bool CameraManager::Terminate()
{
	// ゲームカメラの終了と解放
	if(_gameCamera)
	{
		_gameCamera->Terminate();
		_gameCamera.reset();
	}
	// デバッグカメラの終了と解放
	if(_debugCamera)
	{
		_debugCamera->Terminate();
		_debugCamera.reset();
	}
	return true;
}

bool CameraManager::Process()
{
	// アクティブカメラの更新
	if(_activeCameraType == CameraType::GAME && _gameCamera)
	{
		_gameCamera->SetInput(_key, _trg, _lx, _ly, _rx, _ry, _analogMin);
		_gameCamera->Process();
	}
	else if(_activeCameraType == CameraType::DEBUG && _debugCamera)
	{
		_debugCamera->SetInput(_key, _trg, _lx, _ly, _rx, _ry, _analogMin);
		_debugCamera->Process();
	}
	return true;
}

bool CameraManager::Render()
{
	// アクティブカメラの描画
	if(_activeCameraType == CameraType::GAME && _gameCamera)
	{
		_gameCamera->Render();
	}
	else if(_activeCameraType == CameraType::DEBUG && _debugCamera)
	{
		_debugCamera->Render();
	}
	return true;
}

// カメラ切り替え
void CameraManager::SwitchCamera(CameraType type)
{
	_activeCameraType = type;
}

void CameraManager::ToggleCamera()
{
	if(_activeCameraType == CameraType::GAME)
	{
		_activeCameraType = CameraType::DEBUG;
	}
	else
	{
		_activeCameraType = CameraType::GAME;
	}
}

// 現在のアクティブカメラの情報を取得
VECTOR CameraManager::GetCameraPos() const
{
	if(_activeCameraType == CameraType::GAME && _gameCamera)
	{
		return _gameCamera->GetVPos();
	}
	else if(_activeCameraType == CameraType::DEBUG && _debugCamera)
	{
		return _debugCamera->GetVPos();
	}
	return VGet(0, 0, 0);
}

VECTOR CameraManager::GetCameraTarget() const
{
	if(_activeCameraType == CameraType::GAME && _gameCamera)
	{
		return _gameCamera->GetVTarget();
	}
	else if(_activeCameraType == CameraType::DEBUG && _debugCamera)
	{
		return _debugCamera->GetVTarget();
	}
	return VGet(0, 0, 0);
}

// ターゲット設定（ゲームカメラ用）
void CameraManager::SetTarget(std::shared_ptr<PlayerBase> target)
{
	if(_gameCamera)
	{
		_gameCamera->SetTarget(target);
	}
}

// カメラ設定を適用
void CameraManager::ApplyCameraSettings()
{
	if(_activeCameraType == CameraType::GAME && _gameCamera)
	{
		_gameCamera->SetUp();
	}
	else if(_activeCameraType == CameraType::DEBUG && _debugCamera)
	{
		_debugCamera->SetUp();
	}
}