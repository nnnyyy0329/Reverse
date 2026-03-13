#include "EventCamera.h"

void EventCamera::Process()
{
	// 基準位置にカメラを戻す
	_vPos = _vBasePos;
	_vTarget = _vBaseTarget;
}

void EventCamera::OnEnter()
{
	// ==================================================
	// 1. ターゲット（注視点）を足元に下げる
	// GameCameraの _vTarget は胸の高さ(Y+150)なので、Y座標を引いて足元に向けます
	_vTarget.y -= 100.0f;
	_vBaseTarget = _vTarget;

	// 2. 「見下ろす角度」を強制的にセットする
	// マイナス（見上げ）だった角度をプラス（見下ろし）に書き換えます
	_fAngleV = 0.5f;

	// 3. カメラの距離をセット
	_fDistance = 400.0f;

	// 4. ★最重要：正しいカメラ座標を「再計算」する！
	// GameCameraから引き継いだバグ座標(_vPos)を捨てて、
	// 上記で設定した「足元の注視点」「見下ろし角度」「背後の角度」から
	// 完璧なカメラ位置(_vPos)をここで作り直します！
	UpdatePosFromAngle();
	_vBasePos = _vPos;
	// ==================================================

	// 5. ズームする方向ベクトルを計算して記憶
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
