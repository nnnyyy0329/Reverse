#pragma once
#include "appframe.h"

class PlayerBase;

class GameCamera
{
public:
	GameCamera();
	virtual ~GameCamera() {};

	void Process(int key, int trg, float lx, float ly, float rx, float ry, float analogMin, bool isInput);
	void DebugRender();

	void SetUp();															// カメラ設定
	void UpdateCamera();													// カメラの更新処理	
	void ControlCamera(float rx, float ry, float analogMin, bool isInput);	// カメラ操作処理

	// ゲッター
	VECTOR GetVPos() const { return _vPos; }
	VECTOR GetVTarget() const { return _vTarget; }

	// ターゲットを設定する関数
	void SetTarget(std::shared_ptr<PlayerBase> target);

protected:
	VECTOR _vPos;
	VECTOR _vTarget;		// 注視点
	float _nearClip;		// 描画する手前の距離
	float _farClip;			// 描画する奥の距離
	VECTOR _posOffset;		// カメラ位置のオフセット
	VECTOR _targetOffset;	// 注視点のオフセット

	// ターゲットとなるゲームオブジェクト
	std::shared_ptr<PlayerBase> _targetObject;

	// 入力状態
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;
};
