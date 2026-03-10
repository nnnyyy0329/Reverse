#include "GameCamera.h"
#include "PlayerBase.h"

namespace
{
	constexpr float ROTATE_SPEED = 0.035f;// 回転速度
	constexpr float ANGLE_V_LIMIT = 1.50f;// 垂直角度制限(真上、真下の反転防止。85度)
	constexpr float TARGET_OFFSET_Y = 150.0f;// ターゲットの注視点のYオフセット
}

GameCamera::GameCamera()
{
	_vPosOffset = VGet(0.0f, TARGET_OFFSET_Y, 0.0f);
	_fNearClip = 1.f;
	_fFarClip = 5000.f;
	_fDistance = 450.0f;
	_fAngleH = 0.0f;
	_fAngleV = 0.3f;// 少し見下ろし
}

void GameCamera::Process()
{
	// カメラ操作処理
	ControlCamera();

	// カメラ更新処理
	UpdateCamera();
}

// デバッグ用描画処理
void GameCamera::DebugRender()
{
	int x = 400;
	int y = 50;
	// 座標系表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "GameCamera Pos:    (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
	DrawFormatString(x, y + 20, GetColor(255, 255, 255), "GameCamera Target: (%3.2f, %3.2f, %3.2f)", _vTarget.x, _vTarget.y, _vTarget.z);
	DrawFormatString(x, y + 40, GetColor(255, 255, 255), "AngleH: %3.2f  AngleV: %3.2f  Dist: %3.2f", _fAngleH, _fAngleV, _fDistance);
}

void GameCamera::SetTarget(std::shared_ptr<PlayerBase> target)
{
	_targetObject = target;

	if (_targetObject)
	{
		// 注視点を初期化
		_vTarget = VAdd(_targetObject->GetPos(), _vPosOffset);

		// 角度、距離から座標を計算
		UpdatePosFromAngle();
	}
}

void GameCamera::ApplyShake(const VECTOR& shakeOffset)
{
	_vPos = VAdd(_vPos, shakeOffset);
	_vTarget = VAdd(_vTarget, shakeOffset);
}

// 注視点から後方へdistanceだけ離れた位置にカメラを配置する
void GameCamera::UpdateCamera()
{
	if (!_targetObject) { return; }

	// 注視点を決定
	_vTarget = VAdd(_targetObject->GetPos(), _vPosOffset);

	// 角度から前方ベクトルを計算
	float cosV = cos(_fAngleV);
	float sinV = sin(_fAngleV);
	float cosH = cos(_fAngleH);
	float sinH = sin(_fAngleH);
	VECTOR forward = VGet(cosV * sinH, sinV, cosV * cosH);

	_vPos = VSub(_vTarget, VScale(forward, _fDistance));
}

// スティック入力でカメラ角度を更新
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
