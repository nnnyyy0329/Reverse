#pragma once
#include "DxLib.h"

class DebugCamera
{
public:
	struct StickInput {
		float lx;// 左スティック
		float ly;
		float rx;// 右スティック
		float ry;
		float analogMin;// アナログスティックのデッドゾーン閾値
	};

	DebugCamera();
	virtual ~DebugCamera();
	void Process(const StickInput& input);
	void Render();

	// 元のカメラ、元のターゲット位置からデバッグカメラを設定する
	void SetInfo(const VECTOR& originCamPos, const VECTOR& originTargetPos);

	void SetUp();// DxLibカメラへ反映させる

	float ApplyDeadZone(float value, float deadZone);// デッドゾーンを適用する

protected:
	VECTOR _vPos;// カメラ位置
	VECTOR _vTarget;// 注視点
	VECTOR _vDebugObjPos;// デバッグ用の仮オブジェックとの位置

	float _nearClip;// 描画する手前の距離
	float _farClip;// 描画する奥の距離

	VECTOR _vPosOffset;// カメラ位置のオフセット
	VECTOR _vTargetOffset;// 注視点のオフセット

	MATRIX _matRotY;

	float _yaw;// Y軸回転角(ラジアン)を保持
};

