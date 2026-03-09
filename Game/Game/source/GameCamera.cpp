#include "GameCamera.h"
#include "PlayerBase.h"
#include "CameraShakeSystem.h"

namespace
{
	constexpr float ROTATE_SPEED = 0.035f;// 回転速度
	constexpr float ANGLE_V_LIMIT = 1.50f;// 垂直角度制限(真上、真下の反転防止。85度)

	constexpr float TARGET_OFFSET_Y = 150.0f;// ターゲットの注視点のYオフセット
}

GameCamera::GameCamera()
{
	_fNearClip = 1.f;
	_fFarClip = 5000.f;
	_fDistance = 450.0f;
}

void GameCamera::Process()
{

	// カメラ更新処理
	UpdateCamera();

	// カメラ位置の更新
	UpdateCameraPos();

	// カメラシェイクオフセットの設定処理
	SetShakeOffset();

	// カメラ操作処理
	ControlCamera(rx, ry, analogMin);
}

// デバッグ用描画処理
void GameCamera::DebugRender()
{
	int x = 400;
	int y = 50;
	// 座標系表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "GameCamera Pos: (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
}

// カメラシェイクオフセットの設定処理
void GameCamera::SetShakeOffset()
{
	// 振動オフセットを取得
	VECTOR shakeOffset = VGet(0.0f, 0.0f, 0.0f);

	// カメラシェイクシステムが存在し、振動中であればオフセットを取得
	if(_cameraShakeSystem && _cameraShakeSystem->IsShaking())
	{
		// カメラシェイクオフセットを取得
		shakeOffset = _cameraShakeSystem->GetShakeOffset();
	}

	// 振動オフセットを適用したカメラ位置を計算
	VECTOR finalCameraPos = VAdd(_baseOffset, shakeOffset);

	// カメラ位置を更新
	_vPos = finalCameraPos;
}

// 更新処理
void GameCamera::UpdateCamera()
{
	// カメラ位置と注視点を更新
	if(_targetObject)
	{
		// ターゲットの位置を取得
		VECTOR targetPos = _targetObject->GetPos();

		// 位置更新
		VECTOR newPos = VAdd(targetPos, _posOffset);
		_vPos = newPos;

		// 注視点更新
		VECTOR newTarget = VAdd(targetPos, _targetOffset);
		_vTarget = newTarget;
	}
}

// カメラ操作処理
void GameCamera::ControlCamera()
{
	auto& im = InputManager::GetInstance();
	const AnalogState& analog = im.GetAnalog();
	float analogMin = im.GetAnalogMin();
	float rx = analog.rx;
	float ry = analog.ry;

	// カメラの回転
	{
		// 水平回転
		if(abs(rx) > analogMin)
		{
			_fAngleH += rx * ROTATE_SPEED;// 右スティックのX軸で水平回転
		}

		// 垂直回転
		if(abs(ry) > analogMin) 
		{
			_fAngleV -= ry * ROTATE_SPEED;// 右スティックのY軸で垂直回転

			// 垂直角度制限
			if(_fAngleV > ANGLE_V_LIMIT) _fAngleV = ANGLE_V_LIMIT;
			if(_fAngleV < -ANGLE_V_LIMIT) _fAngleV = -ANGLE_V_LIMIT;
		}
	}
}

void GameCamera::SetTarget(std::shared_ptr<PlayerBase> target)
{
	_targetObject = target;

	// カメラ位置と注視点の初期設定
	if(_targetObject)
	{
		const VECTOR targetPos = _targetObject->GetPos();
		_vPos = VAdd(targetPos, _posOffset);
		_vTarget = VAdd(targetPos, _targetOffset);
	}
}
