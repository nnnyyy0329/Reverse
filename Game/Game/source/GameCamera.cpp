#include "GameCamera.h"
#include "PlayerBase.h"

namespace
{
	constexpr float ROTATE_SPEED = 0.045f;// 回転速度
	constexpr float ANGLE_V_LIMIT = 0.75f;// 垂直角度制限(真上、真下の反転防止。85度)
	constexpr float TARGET_OFFSET_Y = 150.0f;// ターゲットの注視点のYオフセット
	constexpr float DEFAULT_DISTANCE = 300.0f;// デフォルトのカメラ距離
	constexpr float DEFAULT_ANGLE_H = DX_PI_F;// デフォルトの水平角度(真後ろ)
	constexpr float DEFAULT_ANGLE_V = -0.2f;// デフォルトの垂直角度(少し見下ろし)

	constexpr float AUTO_FOLLOW_TIME = 30.0f;// 自動追従開始までのフレーム
}

GameCamera::GameCamera()
{
	_fNearClip = 1.f;
	_fFarClip = 10000.f;
	_fDistance = DEFAULT_DISTANCE;
	_fAngleH = DEFAULT_ANGLE_H;
	_fAngleV = -DEFAULT_ANGLE_V;// 少し見下ろし

	_lookAtSettings.worldOffset = VGet(0.0f, TARGET_OFFSET_Y, 0.0f);
	_lookAtSettings.localOffset = VGet(0.0f, 0.0f, 0.0f);
	_lookAtSettings.lerpRate = 1.0f;
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
		_vTarget = CalculateLookAtPoint();
	}
}

void GameCamera::ApplyShake(const VECTOR& shakeOffset)
{
	_vPos = VAdd(_vPos, shakeOffset);
	_vTarget = VAdd(_vTarget, shakeOffset);
}

void GameCamera::Reset()
{
	// 角度と距離をデフォルトに戻す
	_fDistance = DEFAULT_DISTANCE;
	_fAngleV = DEFAULT_ANGLE_V;
	_fAutoFollowTimer = 0.0f;

	if (_targetObject)
	{
		// ターゲットの真後ろにセット
		_fAngleH = _targetObject->GetRotY();
		_vTarget = CalculateLookAtPoint();
	}
	else
	{
		_fAngleH = DEFAULT_ANGLE_H;
	}

	_fAngleH = NormalizeAngleRad(_fAngleH);
	UpdatePosFromAngle();
}

// 注視点から後方へdistanceだけ離れた位置にカメラを配置する
void GameCamera::UpdateCamera()
{
	if (!_targetObject) { return; }

	// 右スティックの入力チェック
	auto& im = InputManager::GetInstance();
	const AnalogState& analog = im.GetAnalog();
	bool isCameraOperated = (abs(analog.rx) > 0.1f || abs(analog.ry) > 0.1f);

	// 左スティックの入力の大きさを計算
	float moveInputMag = sqrtf(analog.lx * analog.lx + analog.ly * analog.ly);

	//// カメラの自動追従処理
	//// 移動中 && 入力が強い && カメラ操作なし
	//if (_targetObject->IsMoving() && moveInputMag > 0.5f && !isCameraOperated)
	//{
	//	_fAutoFollowTimer++;

		if (_fAutoFollowTimer > AUTO_FOLLOW_TIME)
		{

			// 水平方向
			// 現在のカメラとの角度差を計算
			float targetRot = _targetObject->GetRotY();
			float diff = NormalizeAngleRad(targetRot - _fAngleH);

			float rotSpeed = diff * 0.03f;
			// 回転速度の上限を設定
			float maxRotSpeed = DX_PI_F / 180.0f;
			if (rotSpeed > maxRotSpeed) { rotSpeed = maxRotSpeed; }
			if (rotSpeed < -maxRotSpeed) { rotSpeed = -maxRotSpeed; }

			_fAngleH += rotSpeed;
			_fAngleH = NormalizeAngleRad(_fAngleH);

	//		// 垂直方向
	//		// デフォルトの角度との差を計算
	//		float diffV = DEFAULT_ANGLE_V - _fAngleV;

			float rotSpeedV = diffV * 0.03f;
			float maxRotSpeedV = DX_PI_F / 180.0f;
			if (rotSpeedV > maxRotSpeedV) { rotSpeedV = maxRotSpeedV; }
			if (rotSpeedV < -maxRotSpeedV) { rotSpeedV = -maxRotSpeedV; }

			_fAngleV += rotSpeedV;
			_fAngleV = ClampVerticalAngle(_fAngleV, ANGLE_V_LIMIT);

			// カメラ距離
			// デフォルトの距離との差を計算
			float diffDist = DEFAULT_DISTANCE - _fDistance;
			_fDistance += diffDist * 0.03f;
		}
	}
	else
	{
		_fAutoFollowTimer = 0.0f;
	}

	// 注視点を計算して、必要なら補完する
	VECTOR desiredTarget = CalculateLookAtPoint();
	float t = _lookAtSettings.lerpRate;
	if (t < 0.0f) { t = 0.0f; }
	if (t > 1.0f) { t = 1.0f; }

	_vTarget = VAdd(VScale(_vTarget, 1.0f - t), VScale(desiredTarget, t));

	// 角度と距離からカメラ位置を計算
	UpdatePosFromAngle();
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
			_fAngleH = NormalizeAngleRad(_fAngleH);
		}

		// 垂直回転
		if(abs(ry) > analogMin) 
		{
			_fAngleV -= ry * ROTATE_SPEED;// 右スティックのY軸で垂直回転
			_fAngleV = ClampVerticalAngle(_fAngleV, ANGLE_V_LIMIT);
		}
	}
}

VECTOR GameCamera::CalculateLookAtPoint()
{
	if (!_targetObject) { return _vTarget; }

	// ターゲットの位置にワールドオフセットを加算
	VECTOR lookAt = VAdd(_targetObject->GetPos(), _lookAtSettings.worldOffset);

	// ターゲットの向きからそれぞれの軸を計算
	float rotY = _targetObject->GetRotY();
	VECTOR right = CalcRight(rotY);
	VECTOR up = VGet(0.0f, 1.0f, 0.0f);
	VECTOR forward = VGet(sinf(rotY), 0.0f, cosf(rotY));

	// ローカルオフセットを加算
	lookAt = VAdd(lookAt, VScale(right, _lookAtSettings.localOffset.x));
	lookAt = VAdd(lookAt, VScale(up, _lookAtSettings.localOffset.y));
	lookAt = VAdd(lookAt, VScale(forward, _lookAtSettings.localOffset.z));

	return lookAt;
}
