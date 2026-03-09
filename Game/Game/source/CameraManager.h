#pragma once
#include "appframe.h"

class CameraBase;
class GameCamera;
class DebugCamera;
class AimCamera;

class CameraShakeSystem;

class PlayerBase;

enum class CAMERA_TYPE
{
	NONE,
	GAME_CAMERA,// ゲームカメラ
	DEBUG_CAMERA,// デバッグカメラ
	AIM_CAMERA,// エイムカメラ
	_EOT_,
};

class CameraManager
{
	public:
	CameraManager();
	virtual ~CameraManager();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();
	bool DebugRender();
	void SetUp();

	void SetTarget(std::shared_ptr<PlayerBase> target);

	// カメラ切り替え
	void SetCameraType(CAMERA_TYPE type);
	CAMERA_TYPE GetCameraType() { return _eCameraType; }

	// エイムモード関連
	void StartAimMode();// エイムモード開始
	void EndAimMode();// エイムモード終了
	bool IsAimMode();// エイムモード中か
	VECTOR GetAimDirection();// エイム方向取得

	// デバッグカメラ関連
	void SetIsUseDebugCamera(bool isUse) { _bIsUseDebugCamera = isUse; }
	bool GetIsUseDebugCamera() { return _bIsUseDebugCamera; }

	// カメラシェイク関連
	void StartCameraShake(float magnitude, float duration);
	void StopCameraShake();
	bool IsCameraShaking();

	float GetCurrentCameraAngleH();

protected:
	void ChangeActiveCamera(CAMERA_TYPE type);

	// カメラインスタンスはマネージャーが管理
	std::unique_ptr<GameCamera> _gameCamera;
	std::unique_ptr<DebugCamera> _debugCamera;
	std::unique_ptr<AimCamera> _aimCamera;

	std::unique_ptr<CameraShakeSystem> _cameraShakeSystem;

	CameraBase* _pActiveCamera;

	CAMERA_TYPE _eCameraType;
	CAMERA_TYPE _ePrevCameraType;
	bool _bIsUseDebugCamera;
};

