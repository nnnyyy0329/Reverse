#include "DebugCamera.h"
#include "MyMath.h"

namespace {
	constexpr auto CAMERA_MOVEMENT_SPEED = 8.0f;// 仮オブジェクト移動速度
	constexpr auto CAMERA_ROTATION_SPEED = 2.0f;// カメラ回転速度(度/フレーム)
}

DebugCamera::DebugCamera() {
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);
	_vDebugObjPos = VGet(0.0f, 0.0f, 0.0f);

	_nearClip = 2.0f;
	_farClip = 5000.0f;

	_vPosOffset = VGet(0.0f, 300.0f, 300.0f);
	_vTargetOffset = VGet(0.0f, 150.0f, 0.0f);

	_matRotY = MGetIdent();

	_yaw = 0.0f;
}

DebugCamera::~DebugCamera() {
}

void DebugCamera::Process(const StickInput& input) {
	VECTOR vMove = VGet(0.0f, 0.0f, 0.0f);
	bool isMove = false;

	// デッドゾーンを適用する
	const auto lx = ApplyDeadZone(input.lx, input.analogMin);
	const auto ly = ApplyDeadZone(input.ly, input.analogMin);
	const auto rx = ApplyDeadZone(input.rx, input.analogMin);
	const auto ry = ApplyDeadZone(input.ry, input.analogMin);

	// 仮オブジェクトの移動（左スティック）
	{
		// 入力があれば移動量ベクトルに加算して、移動フラグを立てる
		if (lx > 0.0f) { vMove.x += CAMERA_MOVEMENT_SPEED; isMove = true; }
		if (lx < 0.0f) { vMove.x -= CAMERA_MOVEMENT_SPEED; isMove = true; }
		if (ly > 0.0f) { vMove.z += CAMERA_MOVEMENT_SPEED; isMove = true; }
		if (ly < 0.0f) { vMove.z -= CAMERA_MOVEMENT_SPEED; isMove = true; }

		if (isMove) {
			_vDebugObjPos = VAdd(_vDebugObjPos, vMove);// 仮オブジェクト位置を移動させる
		}
	}

	// カメラ回転角を保持（右スティック）
	{
		// 入力があれば回転量を加算する(degree -> radian)
		if (rx > 0.0f) { _yaw += (CAMERA_ROTATION_SPEED * DEGREE_TO_RADIAN); }
		if (rx < 0.0f) { _yaw -= (CAMERA_ROTATION_SPEED * DEGREE_TO_RADIAN); }

		// 行列を更新する
		_matRotY = MGetRotY(_yaw);
	}

	// カメラ位置と注視点を仮ターゲットの位置にオフセットを加算して設定
	{
		// 位置オフセットはY軸回転角を反映させる
		VECTOR vRotatedPosOffset = VTransform(_vPosOffset, _matRotY);
		VECTOR newPos = VAdd(_vDebugObjPos, vRotatedPosOffset);
		_vPos = newPos;

		// 注視点はオフセットのみ加算
		VECTOR newTarget = VAdd(_vDebugObjPos, _vTargetOffset);
		_vTarget = newTarget;
	}

	// カメラ移動を行う（スティック上下）
	{
		//if (ry > analogMin) { movement += CAMERA_MOVEMENT_SPEED; }
		//if (ry < -analogMin) { movement -= CAMERA_MOVEMENT_SPEED; }

		//if (movement != 0.0f) {
		//	// カメラ位置からターゲットへのベクトルを作成
		//	VECTOR cameraDirection = VSub(_vTarget, _vPos);
		//	VECTOR dir = VNorm(cameraDirection);// 単位化する
		//	// 移動量を掛け算して移動分のベクトルを作成して、カメラの位置ベクトルに足す
		//	VECTOR cameraMovedPos = VAdd(_vPos, VScale(dir, movement));
		//	_vPos = cameraMovedPos;// 移動させる
		//}

		//// カメラ回転を行う（スティック左右）
		//// Y軸回転
		//if (rx > analogMin) { angle += CAMERA_ROTATION_SPEED; }
		//if (rx < -analogMin) { angle -= CAMERA_ROTATION_SPEED; }

		//if (angle != 0.0f) {
		//	// Y軸の回転値を度(ディグリー)で指定して行列で取得する
		//	_matRotY = MGetRotY(angle * DEGREE_TO_RADIAN);
		//	// ターゲットからカメラ位置へのベクトルを計算
		//	VECTOR modelToCamera = VSub(_vPos, _vTarget);
		//	// ターゲットからカメラへのベクトルを回転させて、そのあとにターゲット位置に戻す
		//	_vPos = VAdd(VTransform(modelToCamera, _matRotY), _vTarget);
		//}
	}
}

void DebugCamera::Render() {
	// カメラターゲットを中心に線を引く
	auto lineLength = 10.0f;
	VECTOR vLine = _vTarget;
	DrawLine3D(VAdd(vLine, VGet(-lineLength, 0, 0)), VAdd(vLine, VGet(lineLength, 0, 0)), GetColor(255, 0, 0));
	DrawLine3D(VAdd(vLine, VGet(0, -lineLength, 0)), VAdd(vLine, VGet(0, lineLength, 0)), GetColor(0, 255, 0));
	DrawLine3D(VAdd(vLine, VGet(0, 0, -lineLength)), VAdd(vLine, VGet(0, 0, lineLength)), GetColor(0, 0, 255));
}

void DebugCamera::SetInfo(const VECTOR& originCamPos, const VECTOR& originTargetPos) {
	// デバッグ用の仮オブジェクトを元ターゲットから生成する
	_vDebugObjPos = originTargetPos;
	_vTarget = _vDebugObjPos;

	// 元のカメラ位置からデバッグカメラの位置を設定する
	_vPos = originCamPos;
}

void DebugCamera::SetUp() {
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
}

float DebugCamera::ApplyDeadZone(float value, float deadZone) {
	if (value > deadZone) { return value; }
	else if (value < -deadZone) { return value; }
	return 0.0f;
}
