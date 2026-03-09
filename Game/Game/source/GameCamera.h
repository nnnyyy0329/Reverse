#pragma once
#include "CameraBase.h"

class CameraShakeSystem;

// ゲームカメラクラス
class GameCamera : public CameraBase
{
public:
	GameCamera();
	virtual ~GameCamera() {};

	void Process() override;
	void DebugRender() override;

	void SetShakeOffset();										// カメラシェイクオフセット設定
	void UpdateCamera();										// カメラの更新処理	
	void ControlCamera();



	void SetVPos(const VECTOR& pos) { _vPos = pos; }// カメラ位置を設定

	// ターゲットを設定する関数
	void SetTarget(std::shared_ptr<PlayerBase> target);

	// カメラシェイクシステムを設定する関数
	void SetCameraShakeSystem(std::shared_ptr<CameraShakeSystem> cameraShakeSystem) { _cameraShakeSystem = cameraShakeSystem; }

protected:
	// カメラシェイクシステム
	std::shared_ptr<CameraShakeSystem> _cameraShakeSystem;	

	// ターゲットとなるゲームオブジェクト
	std::shared_ptr<PlayerBase> _targetObject;
};
