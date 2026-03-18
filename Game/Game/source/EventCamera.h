#pragma once
#include "CameraBase.h"

class EventCamera : public CameraBase
{
public:
	EventCamera();
	virtual ~EventCamera() {};

	void Process() override;
	void DebugRender() override;

	void OnEnter() override;

	virtual void SetTarget(std::shared_ptr<PlayerBase> target) {};

	virtual void ApplyShake(const VECTOR& shakeOffset) {};

	VECTOR GetCameraDir() override { return _vBaseDir; }

private:
	VECTOR _vBasePos;// アクティブになったときの基準位置
	VECTOR _vBaseTarget;// アクティブになったときの注視点
	VECTOR _vBaseDir;

};

