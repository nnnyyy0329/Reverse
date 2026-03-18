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
	virtual void SetUp();

	// カメラ切り替え時に呼ばれる
	virtual void OnEnter() {};
	virtual void OnExit() {};

	// ターゲット設定が必要なカメラだけオーバーライド
	virtual void SetTarget(std::shared_ptr<PlayerBase> target) = 0;

	virtual void ApplyShake(const VECTOR& shakeOffset) = 0;

	virtual void Reset() {};

	// ゲッター
	VECTOR GetPos() { return _vPos; }
	VECTOR GetTarget() { return _vTarget; }
	virtual float GetAngleH() { return _fAngleH; }
	float GetAngleV() { return _fAngleV; }
	virtual VECTOR GetCameraDir();// カメラが向いている方向

	// セッター
	void SetPos(const VECTOR& pos) { _vPos = pos; }
	void SetLookAtPoint(const VECTOR& target) { _vTarget = target; }
	void SetAngleH(float angleH) { _fAngleH = angleH; }
	void SetAngleV(float angleV) { _fAngleV = angleV; }

protected:
	void UpdatePosFromAngle();// 角度と距離からカメラ座標を計算
	void CalcAngleFromPos();// カメラ座標と注視点から角度と距離を逆算

	static VECTOR CalcForward(float angleH, float angleV);// 水平、垂直角度から前方ベクトルを生成
	static VECTOR CalcRight(float angleH);// 水平角度から右方向ベクトルを生成
	static float NormalizeAngleRad(float angle);// 角度を-PI～+PIの範囲に正規化
	static float ClampVerticalAngle(float angleV, float limit);// 垂直角度のクランプ

	VECTOR _vPos;
	VECTOR _vTarget;
	VECTOR _vPosOffset;

	float _fNearClip;
	float _fFarClip;
	float _fDistance;
	float _fAngleH;
	float _fAngleV;
};

