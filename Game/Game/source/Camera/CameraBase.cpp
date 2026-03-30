#include "CameraBase.h"

CameraBase::CameraBase()
{
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);
	_vPosOffset = VGet(0.0f, 0.0f, 0.0f);
	_fNearClip = 0.0f;
	_fFarClip = 0.0f;
	_fDistance = 0.0f;
	_fAngleH = 0.0f;
	_fAngleV = 0.0f;
}

void CameraBase::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	// カメラの描画距離を設定
	SetCameraNearFar(_fNearClip, _fFarClip);
}

void CameraBase::UpdatePosFromAngle()
{
	// 注視点から、前方の逆方向に距離を引いてカメラ位置を計算
	VECTOR forward = CalcForward(_fAngleH, _fAngleV);
	_vPos = VSub(_vTarget, VScale(forward, _fDistance));
}

void CameraBase::CalcAngleFromPos()
{
	// 注視点からカメラ位置
	VECTOR diff = VSub(_vPos, _vTarget);

	_fDistance = VSize(diff);
	if (_fDistance < 0.001f)
	{
		_fDistance = 0.001f;
		_fAngleH = 0.0f;
		_fAngleV = 0.0f;
		return;
	}

	VECTOR forward = VScale(diff, -1.0f / _fDistance);

	// 前方ベクトルから水平、垂直角度を逆算
	_fAngleH = atan2f(forward.x, forward.z);
	_fAngleV = asinf(forward.y);
}

VECTOR CameraBase::CalcForward(float angleH, float angleV)
{
	float cosV = cosf(angleV);
	float sinV = sinf(angleV);
	float cosH = cosf(angleH);
	float sinH = sinf(angleH);

	return VGet(cosV * sinH, sinV, cosV * cosH);
}

VECTOR CameraBase::CalcRight(float angleH)
{
	return VGet(cosf(angleH), 0.0f, -sinf(angleH));
}

float CameraBase::NormalizeAngleRad(float angle)
{
	while (angle >= DX_PI_F) { angle -= DX_PI_F * 2.0f; }
	while (angle < -DX_PI_F) { angle += DX_PI_F * 2.0f; }
	return angle;
}

float CameraBase::ClampVerticalAngle(float angleV, float limit)
{
	if (angleV > limit) { return limit; }
	if (angleV < -limit) { return -limit; }
	return angleV;
}

VECTOR CameraBase::GetCameraDir()
{
	return CalcForward(_fAngleH, _fAngleV);
}
