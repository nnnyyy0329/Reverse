#pragma once
#include "appframe.h"

class PlayerBase;
class GameCamera;
class DebugCamera;
class AimCamera;

enum class CAMERA_TYPE
{
	NONE,
	GAME_CAMERA,	// ゲームカメラ
	DEBUG_CAMERA,	// デバッグカメラ
	AIM_CAMERA,		// エイムカメラ
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

	void SwitchCamera();					// カメラ切り替え
	void SwitchCameraProcess();				// カメラ処理切り替え
	void SwitchCameraSetUp();				// カメラ設定切り替え
	void SwitchCameraRender();				// カメラ描画切り替え
	void SwitchCameraDebugRender();			// カメラデバッグ描画切り替え
	float GetCurrentCameraAngleH() const;	// 現在のカメラの水平角度を取得

	// エイムモード関連
	void StartAimMode();			// エイムモード開始
	void EndAimMode();				// エイムモード終了
	bool IsAimMode()const;			// エイムモード中か
	VECTOR GetAimDirection()const;	// エイム方向取得

	// ゲッターセッター
	CAMERA_TYPE GetCameraType()const { return _eCameraType; }

	void SetPlayer(std::shared_ptr<PlayerBase> player);												// プレイヤー設定

	void SetGameCamera(std::shared_ptr<GameCamera> gameCamera) { _gameCamera = gameCamera; }		// ゲームカメラ設定
	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera) { _debugCamera = debugCamera; }	// デバッグカメラ設定
	void SetAimCamera(std::shared_ptr<AimCamera> aimCamera){ _aimCamera = aimCamera; };				// エイムカメラ設定

	bool GetIsUseDebugCamera()const { return _bIsUseDebugCamera; }			// デバッグカメラ使用フラグ取得
	void SetIsUseDebugCamera(bool bIsUse) { _bIsUseDebugCamera = bIsUse; }	// デバッグカメラ使用フラグ設定

protected:
	std::shared_ptr<GameCamera> _gameCamera;
	std::shared_ptr<DebugCamera> _debugCamera;
	std::shared_ptr<AimCamera> _aimCamera;

	CAMERA_TYPE _eCameraType;		// 現在のカメラタイプ
	CAMERA_TYPE _ePrevCameraType;	// 前回のカメラタイプ

	// デバッグカメラ使用フラグ
	bool _bIsUseDebugCamera;

};

