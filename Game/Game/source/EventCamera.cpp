#include "EventCamera.h"

EventCamera::EventCamera()
{
	_vBasePos = VGet(0.0f, 0.0f, 0.0f);
	_vBaseTarget = VGet(0.0f, 0.0f, 0.0f);
	_vBaseDir = VGet(0.0f, 0.0f, 1.0f);
}

void EventCamera::Process()
{
	// 基準位置、注視点を維持
	_vPos = _vBasePos;
	_vTarget = _vBaseTarget;
}

void EventCamera::DebugRender()
{
	int x = 400;
	int y = 50;
	// 座標系表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "EventCamera Pos:    (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
	DrawFormatString(x, y + 20, GetColor(255, 255, 255), "EventCamera Target: (%3.2f, %3.2f, %3.2f)", _vTarget.x, _vTarget.y, _vTarget.z);
	DrawFormatString(x, y + 40, GetColor(255, 255, 255), "AngleH: %3.2f  AngleV: %3.2f  Dist: %3.2f", _fAngleH, _fAngleV, _fDistance);
}

void EventCamera::OnEnter()
{
	// 切り替え時にマネージャーから引き継いだ
	// 位置、注視点を基準に使う
	_vBasePos = _vPos;
	_vBaseTarget = _vTarget;

	// 演出用の進行方向を保存
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
