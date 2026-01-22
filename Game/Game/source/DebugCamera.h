#pragma once
#include "appframe.h"

class DebugCamera
{
public:
	DebugCamera();
	virtual ~DebugCamera();

	void Process(float lx, float ly, float rx, float ry, float analogMin, bool bIsPut);
	void DebugRender();

	void SetUp();

	void SetInfo(VECTOR vPos, VECTOR vTarget);// 座標と注視点を設定

protected:
	void UpdatePosFromAngle();// 角度と距離から座標を計算
	void CalcAngleFromPos();// 座標から角度と距離を計算

	VECTOR _vPos;
	VECTOR _vTarget;

	float _distance;// 注視点からカメラまでの距離
	float _angleH;// 水平方向の角度(ラジアン)
	float _angleV;// 垂直方向の角度(ラジアン)

	float _nearClip;// 描画する手前の距離
	float _farClip;// 描画する奥の距離
};

