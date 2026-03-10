#pragma once
#include "appframe.h"

class PlayerBase;

class CameraBase
{
public:
	CameraBase();
	virtual ~CameraBase() = default;

	virtual void Process() {};
	virtual void Render() {};
	virtual void DebugRender() {};
	void SetUp();

	// カメラ切り替え時に呼ばれる
	virtual void OnEnter() {};
	virtual void OnExit() {};

	// ターゲット設定が必要なカメラだけオーバーライド
	virtual void SetTarget(std::shared_ptr<PlayerBase> target) = 0;

	virtual void ApplyShake(const VECTOR& shakeOffset) = 0;

	// ゲッター
	VECTOR GetPos() { return _vPos; }
	VECTOR GetTarget() { return _vTarget; }
	virtual float GetAngleH() { return _fAngleH; }
	float GetAngleV() { return _fAngleV; }
	virtual VECTOR GetCameraDir();// カメラが向いている方向
	// セッター
	void SetPos(const VECTOR& pos) { _vPos = pos; }
	void SetAngleH(float angleH) { _fAngleH = angleH; }
	void SetAngleV(float angleV) { _fAngleV = angleV; }

protected:
	void UpdatePosFromAngle();// 角度と距離から座標を計算
	void CalcAngleFromPos();// 座標から角度と距離を計算

	VECTOR _vPos;
	VECTOR _vTarget;
	VECTOR _vPosOffset;

	float _fNearClip;
	float _fFarClip;

	float _fDistance;

	float _fAngleH;
	float _fAngleV;
};

