#pragma once
#include "appframe.h"

class PlayerBase;

class CameraBase
{
public:
	CameraBase();
	virtual ~CameraBase() {};

	virtual void Process() {};
	virtual void Render() {};
	virtual void DebugRender() {};
	void SetUp();

	// ターゲット設定が必要なカメラだけオーバーライド
	virtual void SetTarget(std::shared_ptr<PlayerBase> target) {};

	// ゲッター
	VECTOR GetPos() { return _vPos; }
	VECTOR GetTarget() { return _vTarget; }
	virtual float GetAngleH() { return _fAngleH; }
	float GetAngleV() { return _fAngleV; }

protected:
	void UpdatePosFromAngle();// 角度と距離から座標を計算
	void CalcAngleFromPos();// 座標から角度と距離を計算

	VECTOR _vPos;
	VECTOR _vTarget;

	float _fNearClip;
	float _fFarClip;

	float _fDistance;

	float _fAngleH;
	float _fAngleV;
};

