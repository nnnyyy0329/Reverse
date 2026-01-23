#pragma once
#include "appframe.h"

class GameCamera;
class DebugCamera;

enum class CAMERA_TYPE
{
	NONE,
	GAME_CAMERA,
	DEBUG_CAMERA,
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

	void SwitchCamera();		// カメラ切り替え
	void SwitchCameraProcess();	// カメラ処理切り替え
	void SwitchCameraRender();	// カメラ描画切り替え

	// ゲッターセッター
	CAMERA_TYPE GetCameraType()const { return _eCameraType; }

	void SetGameCamera(std::shared_ptr<GameCamera> gameCamera) { _gameCamera = gameCamera; }		// ゲームカメラ設定
	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { _debugCamera = debugCamera; }	// デバッグカメラ設定

	bool GetIsUseDebugCamera()const { return _bIsUseDebugCamera; }	// デバッグカメラ使用フラグ取得
	void SetIsUseDebugCamera(bool bIsUse) { _bIsUseDebugCamera = bIsUse; }	// デバッグカメラ使用フラグ設定

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}

protected:
	std::shared_ptr<GameCamera> _gameCamera;
	std::shared_ptr<DebugCamera> _debugCamera;

	CAMERA_TYPE _eCameraType;

	// デバッグカメラ使用フラグ
	bool _bIsUseDebugCamera;

	// 入力状態
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;

};

