#pragma once
#include "appframe.h"
#include "ICameraAddon.h"

class CameraBase;
class GameCamera;
class DebugCamera;
class AimCamera;
class EventCamera;

class PlayerBase;

enum class CAMERA_TYPE
{
	NONE,
	GAME_CAMERA,// ゲームカメラ
	DEBUG_CAMERA,// デバッグカメラ
	AIM_CAMERA,// エイムカメラ
	EVENT_CAMERA,// 演出用カメラ
	_EOT_,
};

// カメラ切り替え、カメラ演出を管理するクラス
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

	// デバッグカメラON/OFF
	void SetIsUseDebugCamera(bool isUse);

	VECTOR GetCameraDir();
	float GetCurrentCameraAngleH();

	// アクティブカメラの情報
	VECTOR GetActiveCameraPos();
	VECTOR GetActiveCameraTarget();
	void SetActiveCameraPos(const VECTOR& pos);

	// アドオンを追加する
	void AddAddon(std::shared_ptr<ICameraAddon> addon);

	// カメラ状態、Addonのリセット
	void Reset();

protected:
	// カメラインスタンスはマネージャーが管理
	std::unique_ptr<GameCamera> _gameCamera;
	std::unique_ptr<DebugCamera> _debugCamera;
	std::unique_ptr<AimCamera> _aimCamera;
	std::unique_ptr<EventCamera> _eventCamera;

	// アドオンのリスト
	std::list<std::shared_ptr<ICameraAddon>> _addons;

	CameraBase* _pActiveCamera;

	CAMERA_TYPE _eCameraType;
	CAMERA_TYPE _ePrevCameraType;
};

