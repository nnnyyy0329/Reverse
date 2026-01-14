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
	_distance = 100.0f;
	_angleH = 0.0f;
	_angleV = 0.0f;
	_nearClip = 2.0f;
	_farClip = 50000.0f;

	CalcAngleFromPos();// 初期座標から角度などを計算しておく
}

DebugCamera::~DebugCamera()
{
}

// 左スティック:ターゲットの移動
// 右スティック:カメラの回転
void DebugCamera::Process(float lx, float ly, float rx, float ry, float analogMin)
{
	// カメラの回転
	{
		if (abs(rx) > analogMin) _angleH += rx * ROTATE_SPEED;
		if (abs(ry) > analogMin) {
			_angleV += ry * ROTATE_SPEED;

			// 垂直角度制限
			if (_angleV > ANGLE_V_LIMIT) _angleV = ANGLE_V_LIMIT;
			if (_angleV < -ANGLE_V_LIMIT) _angleV = -ANGLE_V_LIMIT;
		}
	}


	// ターゲットの移動
	{
		if (abs(lx) > analogMin || abs(ly) > analogMin) {
			// カメラの向いている方向のベクトル
			// Y成分は無視してXZ平面上のベクトルにする
			VECTOR vForward = VGet(cos(_angleH), 0.0f, sin(_angleH));
			VECTOR vRight = VGet(cos(_angleH + DX_PI_F / 2.0f), 0.0f, sin(_angleH + DX_PI_F / 2));

			// 移動量を計算
			VECTOR vMove = VAdd(
				VScale(vForward, ly * MOVE_SPEED),
				VScale(vRight, lx * MOVE_SPEED)
			);

			// ターゲット位置を更新
			_vTarget = VAdd(_vTarget, vMove);
		}
	}

	// 最終的ななカメラ位置を計算
	UpdatePosFromAngle();
}

void DebugCamera::DebugRender()
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

void DebugCamera::SetUp()
{
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);// カメラ位置と注視点の設定
	SetCameraNearFar(_nearClip, _farClip);// カメラからどれだけ離れた距離からどこまで描画するかを設定
}

// 角度と距離からカメラの座標を計算
// スティック操作で角度を変えた後に呼ぶ
void DebugCamera::UpdatePosFromAngle()
{
	/*
	* 球面座標系から直交座標系へ変換
	* Y軸(高さ) = 距離 * sin(垂直角度)
	* 水平半径  = 距離 * cos(垂直角度)
	* X軸 = 水平半径 * cos(水平角度)
	* Z軸 = 水平半径 * sin(水平角度)
	*/

	// 1/高さ(y)と水平面でのターゲットまでの距離(r)を計算
	auto y = sin(_angleV) * _distance;
	auto r = cos(_angleV) * _distance;

	// 2.水平平面上での位置(x,z)を計算
	auto x = cos(_angleH) * r;
	auto z = sin(_angleH) * r;

	// 3.相対位置をターゲットの座標に足してカメラの絶対座標を計算
	_vPos = VAdd(_vTarget, VGet(x, y, z));
}

// ターゲットの座標とカメラの座標から角度と距離を計算
// 初期化時や外部からカメラ位置を変更した後に呼ぶ
void DebugCamera::CalcAngleFromPos()
{
	VECTOR vDiff = VSub(_vPos, _vTarget);// ターゲットからカメラへのベクトル

	// 距離
	_distance = VSize(vDiff);
	if (_distance < 0.001f) {// ゼロ除算防止
		_distance = 0.001f;
		_angleH = 0.0f;
		_angleV = 0.0f;
		return;// 距離がほぼ0なら計算しない

	}

	// 水平角度(atan2)
	// x,zから角度(ラジアン)を計算
	_angleH = atan2f(vDiff.z, vDiff.x);

	// 垂直角度(asin)
	// 斜辺(距離)に対する高さ(y)の割合から角度を計算
	_angleV = asinf(vDiff.y / _distance);
}
