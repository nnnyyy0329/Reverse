#include "DebugCamera.h"

namespace {
	constexpr auto MOVE_SPEED = 3.0f;// ターゲット移動速度
	constexpr auto ROTATE_SPEED = 0.02f;// 回転速度
	constexpr auto ZOOM_SPEED = 5.0f;// ズーム速度
	constexpr auto ANGLE_V_LIMIT = 1.50f;// 垂直角度制限(真上、真下の反転防止。85度)
}

DebugCamera::DebugCamera()
{
	_vPos = VGet(0.0f, 0.0f, -100.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);
	_fDistance = 100.0f;
	_fAngleH = 0.0f;
	_fAngleV = 0.0f;
	_fNearClip = 2.0f;
	_fFarClip = 50000.0f;

	CalcAngleFromPos();// 初期座標から角度などを計算しておく
}

DebugCamera::~DebugCamera()
{
}

// 左スティック:ターゲットの移動(ボタン同時押し(B)で高さ変更、ズーム)
// 右スティック:カメラの回転
void DebugCamera::Process()
{
	auto& im = InputManager::GetInstance();
	const AnalogState& analog = im.GetAnalog();
	float analogMin = im.GetAnalogMin();
	float lx = analog.lx;
	float ly = analog.ly;
	float rx = analog.rx;
	float ry = analog.ry;
	bool bIsPut = im.IsHold(INPUT_ACTION::DODGE);

	// カメラの回転
	{
		if (abs(rx) > analogMin) { _fAngleH -= rx * ROTATE_SPEED; }
		if (abs(ry) > analogMin) 
		{
			_fAngleV += ry * ROTATE_SPEED;

			// 垂直角度制限
			if (_fAngleV > ANGLE_V_LIMIT) _fAngleV = ANGLE_V_LIMIT;
			if (_fAngleV < -ANGLE_V_LIMIT) _fAngleV = -ANGLE_V_LIMIT;
		}
	}

	// ターゲットの移動
	{
		if (abs(lx) > analogMin || abs(ly) > analogMin) 
		{

			// ボタン同時押し中(B)
			if (bIsPut)
			{
				// ターゲットの高さ変更
				_vTarget.y -= ly * MOVE_SPEED;

				// ズームイン・アウト
				_fDistance += lx * ZOOM_SPEED;
				if (_fDistance < 1.0f) { _fDistance = 1.0f; }// 最小距離制限
			}
			else // 通常時
			{
				// カメラの向いている方向のベクトル
				// Y成分は無視してXZ平面上のベクトルにする
				VECTOR vForward = VGet(cos(_fAngleH), 0.0f, sin(_fAngleH));
				VECTOR vRight = VGet(cos(_fAngleH + DX_PI_F / 2.0f), 0.0f, sin(_fAngleH + DX_PI_F / 2));

				// 移動量を計算
				VECTOR vMove = VAdd(
					VScale(vForward, ly * MOVE_SPEED),
					VScale(vRight, lx * MOVE_SPEED)
				);

				// ターゲット位置を更新
				_vTarget = VAdd(_vTarget, vMove);
			}
		}
	}

	// 最終的ななカメラ位置を計算
	UpdatePosFromAngle();
}

void DebugCamera::Render()
{
	SetUseZBuffer3D(FALSE);// 壁などに隠れないように

	auto lineLength = 10.0f;
	// ターゲット位置に十字を描画
	DrawLine3D(// X軸
		VAdd(_vTarget, VGet(-lineLength, 0.0f, 0.0f)),
		VAdd(_vTarget, VGet(lineLength, 0.0f, 0.0f)),
		GetColor(255, 0, 0)
	);
	DrawLine3D(// Y軸
		VAdd(_vTarget, VGet(0.0f, -lineLength, 0.0f)),
		VAdd(_vTarget, VGet(0.0f, lineLength, 0.0f)),
		GetColor(0, 255, 0)
	);
	DrawLine3D(// Z軸
		VAdd(_vTarget, VGet(0.0f, 0.0f, -lineLength)),
		VAdd(_vTarget, VGet(0.0f, 0.0f, lineLength)),
		GetColor(0, 0, 255)
	);

	SetUseZBuffer3D(TRUE);// 元に戻す
}

void DebugCamera::DebugRender()
{
	int x = 400;
	int y = 70;

	// 座標系表示
	DrawFormatString(x, y, GetColor(255, 255, 255), "DebugCamera Pos: (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
}

void DebugCamera::SetInfo(VECTOR vPos, VECTOR vTarget)
{
	_vPos = vPos;
	_vTarget = vTarget;
	CalcAngleFromPos();// セットされた座標から角度と距離を再計算
}
