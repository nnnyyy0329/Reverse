#include "GameCamera.h"
#include "PlayerBase.h"

namespace
{
	constexpr auto ROTATE_SPEED = 0.02f;	// 回転速度
	constexpr auto ANGLE_V_LIMIT = 1.50f;	// 垂直角度制限(真上、真下の反転防止。85度)
}

GameCamera::GameCamera()
{
	// カメラの設定
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);

	_nearClip = 1.f;
	_farClip = 5000.f;

	_distance = 450.0f;
	_angleH = 0.0f;
	_angleV = 0.0f;

	//_posOffset = VGet(0.0f, 0.0f, 0.0f);
	//_targetOffset = VGet(0.0f, 100.0f, 0.0f);
	_targetOffset = VGet(0.0f, 150.0f, 0.0f);
}

void GameCamera::Process(int key, int trg, float lx, float ly, float rx, float ry, float analogMin, bool isInput)
{
	// カメラの回転
	{
		if(abs(rx) > analogMin) _angleH += rx * ROTATE_SPEED;
		if(abs(ry) > analogMin) {
			_angleV += ry * ROTATE_SPEED;

			// 垂直角度制限
			if(_angleV > ANGLE_V_LIMIT) _angleV = ANGLE_V_LIMIT;
			if(_angleV < -ANGLE_V_LIMIT) _angleV = -ANGLE_V_LIMIT;
		}
	}

	// カメラ更新処理
	UpdateCamera();

	// カメラ位置の更新
	UpdateCameraPos();

	// カメラ操作処理
	ControlCamera(rx, ry, analogMin);
}

void GameCamera::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);

	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
}

void GameCamera::Render()
{
}

// デバッグ用描画処理
void GameCamera::DebugRender()
{
	int x = 900;
	int y = 410;

	// 座標系表示
	DrawFormatString(x, y, GetColor(55, 0, 0), "GameCamera Pos: (%3.2f, %3.2f, %3.2f)", _vPos.x, _vPos.y, _vPos.z);
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

// カメラ位置の更新
void GameCamera::UpdateCameraPos()
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

// カメラ操作処理
void GameCamera::ControlCamera(float rx, float ry, float analogMin)
{
	//// カメラ操作を行う（右スティック）
	//{
	//	float sx = _vPos.x - _vTarget.x;
	//	float sz = _vPos.z - _vTarget.z;
	//	float rad = atan2(sz, sx);
	//	float length = sqrt(sz * sz + sx * sx);

	//	// XZ位置
	//	if(rx > analogMin) { rad -= 0.05f; }
	//	if(rx < -analogMin) { rad += 0.05f; }

	//	// XZ位置更新
	//	_vPos.x = _vTarget.x + cos(rad) * length;
	//	_vPos.z = _vTarget.z + sin(rad) * length;

	//	// Y位置
	//	if(ry > analogMin) { _vPos.y -= 5.f; }
	//	if(ry < -analogMin) { _vPos.y += 5.f; }
	//}

	// カメラの回転
	{
		if(abs(rx) > analogMin) _angleH += rx * ROTATE_SPEED;
		if(abs(ry) > analogMin) {
			_angleV += ry * ROTATE_SPEED;

			// 垂直角度制限
			if(_angleV > ANGLE_V_LIMIT) _angleV = ANGLE_V_LIMIT;
			if(_angleV < -ANGLE_V_LIMIT) _angleV = -ANGLE_V_LIMIT;
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
