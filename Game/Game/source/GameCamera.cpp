#include "GameCamera.h"
#include "PlayerBase.h"

GameCamera::GameCamera()
{
	// カメラの設定
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);

	_nearClip = 1.f;
	_farClip = 5000.f;

	_posOffset = VGet(0.0f, 200.0f, 500.0f);
	_targetOffset = VGet(0.0f, 100.0f, 0.0f);
}

void GameCamera::Process(int key, int trg, float lx, float ly, float rx, float ry, float analogMin, bool isInput)
{
	// カメラ更新処理
	UpdateCamera();

	// カメラ操作処理
	ControlCamera(rx, ry, analogMin, isInput);
}

void GameCamera::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);

	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
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
void GameCamera::ControlCamera(float rx, float ry, float analogMin, bool isInput)
{
	// カメラ操作を行う（右スティック）
	{
		float sx = _vPos.x - _vTarget.x;
		float sz = _vPos.z - _vTarget.z;
		float rad = atan2(sz, sx);
		float length = sqrt(sz * sz + sx * sx);

		// XZ位置
		if(rx > analogMin) { rad -= 0.05f; }
		if(rx < -analogMin) { rad += 0.05f; }

		// XZ位置更新
		_vPos.x = _vTarget.x + cos(rad) * length;
		_vPos.z = _vTarget.z + sin(rad) * length;

		// Y位置
		if(ry > analogMin) { _vPos.y -= 5.f; }
		if(ry < -analogMin) { _vPos.y += 5.f; }
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
