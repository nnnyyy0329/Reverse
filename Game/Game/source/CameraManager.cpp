#include "CameraManager.h"
#include "CameraBase.h"
#include "GameCamera.h"
#include "DebugCamera.h"
#include "AimCamera.h"
#include "CameraShakeSystem.h"

CameraManager::CameraManager()
{
	_gameCamera = std::make_unique<GameCamera>();
	_debugCamera = std::make_unique<DebugCamera>();
	_aimCamera = std::make_unique<AimCamera>();

	_cameraShakeSystem = std::make_unique<CameraShakeSystem>();

	_pActiveCamera = _gameCamera.get();

	_eCameraType = CAMERA_TYPE::GAME_CAMERA;// 初期タイプはゲームカメラ
	_ePrevCameraType = CAMERA_TYPE::NONE;// 前回のカメラタイプ初期化

	_bIsUseDebugCamera = false;
}

CameraManager::~CameraManager()
{
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
	if (!IsAimMode())
	{
		CAMERA_TYPE next = _bIsUseDebugCamera 
			? CAMERA_TYPE::DEBUG_CAMERA 
			: CAMERA_TYPE::GAME_CAMERA;

		if(_eCameraType != next)
		{
			ChangeActiveCamera(next);
		}
	}

	if (_pActiveCamera)
	{
		_pActiveCamera->Process();
	}

	if(_cameraShakeSystem)
	{
		_gameCamera->SetCameraShakeSystem(_cameraShakeSystem);
		_cameraShakeSystem->Process();
	}

	return true;
}

bool CameraManager::Render()
{
	return true;
}

bool CameraManager::DebugRender()
{
	return true;
}

void CameraManager::SetUp()
{
	if(_pActiveCamera)
	{
		_pActiveCamera->SetUp();
	}
}

void CameraManager::SetTarget(std::shared_ptr<PlayerBase> target)
{
	if (_gameCamera) { _gameCamera->SetTarget(target); }
	if (_aimCamera) { _aimCamera->SetTarget(target); }
}

void CameraManager::ChangeActiveCamera(CAMERA_TYPE type)
{
	_ePrevCameraType = _eCameraType;
	_eCameraType = type;

	switch (type)
	{
	case CAMERA_TYPE::GAME_CAMERA: _pActiveCamera = _gameCamera.get(); break;
	case CAMERA_TYPE::DEBUG_CAMERA: _pActiveCamera = _debugCamera.get(); break;
	case CAMERA_TYPE::AIM_CAMERA: _pActiveCamera = _aimCamera.get(); break;
	default: _pActiveCamera = nullptr; break;
	}
}

void CameraManager::SetCameraType(CAMERA_TYPE type)
{
	ChangeActiveCamera(type);
}

void CameraManager::StartAimMode()
{
	if (!_aimCamera || IsAimMode()) { return; }
	ChangeActiveCamera(CAMERA_TYPE::AIM_CAMERA);
	_aimCamera->StartAiming();
}

void CameraManager::EndAimMode()
{
	if (!_aimCamera || !IsAimMode()) { return; }
	ChangeActiveCamera(_ePrevCameraType);
	_aimCamera->EndAiming();
}

bool CameraManager::IsAimMode()
{
	if (!_aimCamera) { return false; }
	return _aimCamera->IsAiming();
}

VECTOR CameraManager::GetAimDirection()
{
	if(!_aimCamera || !_aimCamera->IsAiming())
	{
		return VGet(0.0f, 0.0f, 0.0f);
	}
	return _aimCamera->GetAimDirection();
}

float CameraManager::GetCurrentCameraAngleH()
{
	if(_pActiveCamera) { return _pActiveCamera->GetAngleH(); }
	return 0.0f;
}

void CameraManager::StartCameraShake(float magnitude, float duration)
{
	if (_cameraShakeSystem) { _cameraShakeSystem->StartShake(magnitude, duration); }
}

void CameraManager::StopCameraShake()
{
	if (_cameraShakeSystem) { _cameraShakeSystem->StopShake(); }
}

bool CameraManager::IsCameraShaking()
{
	if (_cameraShakeSystem) { return _cameraShakeSystem->IsShaking(); }
	return false;
}