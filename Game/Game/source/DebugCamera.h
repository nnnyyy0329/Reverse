#pragma once
#include "CameraBase.h"

class DebugCamera : public CameraBase
{
public:
	DebugCamera();
	virtual ~DebugCamera();

	void Process() override;
	void Render() override;
	void DebugRender() override;

	void SetTarget(std::shared_ptr<PlayerBase> target) override {}
	void ApplyShake(const VECTOR& shakeOffset) override {}

	void SetInfo(VECTOR vPos, VECTOR vTarget);// 座標と注視点を直接セット

protected:

};

