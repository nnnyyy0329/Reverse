#pragma once
#include "appframe.h"

class GameCamera;
class DebugCamera;
class PlayerBase;

// カメラの切り替えクラス
class CameraManager
{
public:

	// カメラタイプ列挙
	enum class CameraType
	{
		GAME,
		DEBUG
	};

	CameraManager();
	virtual ~CameraManager();
	bool Initialize();		
	bool Terminate();		
	bool Process();			
	bool Render();		

	// カメラインスタンスを設定
	void SetGameCamera(std::shared_ptr<GameCamera> gameCamera);
	void SetDebugCamera(std::shared_ptr<DebugCamera> debugCamera);

	// カメラ切り替え
	void SwitchCamera(CameraType type);
	void ToggleCamera();			

	// 現在のアクティブカメラの情報を取得
	VECTOR GetCameraPos() const;
	VECTOR GetCameraTarget() const;

	// ターゲット設定（ゲームカメラ用）
	void SetTarget(std::shared_ptr<PlayerBase> target);

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

	// カメラ設定を適用
	void ApplyCameraSettings();

	// ゲッターセッター
	CameraType GetActiveCameraType() const { return _activeCameraType; }

	// 現在のアクティブカメラの情報を取得
	VECTOR GetCameraPos() const;
	VECTOR GetCameraTarget() const;

protected:
	// カメラインスタンス
	std::shared_ptr<GameCamera> _gameCamera;
	std::shared_ptr<DebugCamera> _debugCamera;

	// アクティブカメラタイプ
	CameraType _activeCameraType;

	// 入力状態
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;
};

