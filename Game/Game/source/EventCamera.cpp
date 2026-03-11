#include "EventCamera.h"

void EventCamera::Process()
{
	// 基準位置にカメラを戻す
	_vPos = _vBasePos;
	_vTarget = _vBaseTarget;
}

void EventCamera::SetUp()
{
	// 現在地を基準位置とする
	_vBasePos = GetPos();
	_vBaseTarget = GetTarget();

	// 注視点への方向ベクトルを計算
	VECTOR dir = VSub(_vBaseTarget, _vBasePos);
	if (VSize(dir) > 0.0f)
	{
		_vBaseDir = VNorm(dir);
	}
	else
	{
		_vBaseDir = VGet(0.0f, 0.0f, 1.0f);
	}
}
