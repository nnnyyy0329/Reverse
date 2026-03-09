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

// 角度と距離からカメラの座標を計算
// スティック操作で角度を変えた後に呼ぶ
void CameraBase::UpdatePosFromAngle()
{
	// x = -sin(H) * r
	// z =  cos(H) * r

	// 1.高さ(y)と水平面でのターゲットまでの距離(r)を計算
	float y = sinf(_fAngleV) * _fDistance;
	float r = cosf(_fAngleV) * _fDistance;

	// 2.水平平面上での位置(x,z)を計算
	float x = -sinf(_fAngleH) * r;
	float z = cosf(_fAngleH) * r;

	// 3.相対位置をターゲットの座標に足してカメラの絶対座標を計算
	_vPos = VAdd(_vTarget, VGet(x, y, z));
}

// ターゲットの座標とカメラの座標から角度と距離を計算
// 初期化時や外部からカメラ位置を変更した後に呼ぶ
void CameraBase::CalcAngleFromPos()
{
	VECTOR diff = VSub(_vPos, _vTarget);// ターゲットからカメラへのベクトル

	// 距離
	_fDistance = VSize(diff);
	if (_fDistance < 0.001f) {// ゼロ除算防止
		_fDistance = 0.001f;
		_fAngleH = 0.0f;
		_fAngleV = 0.0f;
		return;// 距離がほぼ0なら計算しない

	}

	// 水平角度(atan2)
	// x,zから角度(ラジアン)を計算
	_fAngleH = atan2f(-diff.z, diff.x);

	// 垂直角度(asin)
	// 斜辺(距離)に対する高さ(y)の割合から角度を計算
	_fAngleV = asinf(diff.y / _fDistance);
}
