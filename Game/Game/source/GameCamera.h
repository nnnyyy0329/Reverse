#pragma once
#include "CameraBase.h"


// ゲームカメラクラス
class GameCamera : public CameraBase
{
public:
	GameCamera();
	virtual ~GameCamera() {};

	void Process() override;
	void DebugRender() override;	

	void SetTarget(std::shared_ptr<PlayerBase> target) override;

	void ApplyShake(const VECTOR& shakeOffset) override;

	void Reset() override;

protected:
	void UpdateCamera();
	void ControlCamera();

	// ターゲットとなるゲームオブジェクト
	std::shared_ptr<PlayerBase> _targetObject;
};
