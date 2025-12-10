#include "CameraManager.h"
#include "PlayerBase.h"

CameraManager::CameraManager()
{
	// カメラの設定（わかりやすい位置に）
	_vPos = VGet(0.0f, 0.0f, 0.0f);
	_vTarget = VGet(0.0f, 0.0f, 0.0f);
	_nearClip = 1.f;
	_farClip = 5000.f;
	_posOffset = VGet(0.0f, 200.0f, 500.0f);
	_targetOffset = VGet(0.0f, 100.0f, 0.0f);
}

void CameraManager::Process()
{
	// ターゲットオブジェクトがあれば、カメラ位置と注視点を更新
	if (_targetObject) {
		// ターゲットの位置を取得
		VECTOR targetPos = _targetObject->GetPos();

		// カメラ位置と注視点をターゲットの位置にオフセットを加算して設定
		// Y座標は変更しない
		VECTOR newPos = VAdd(targetPos, _posOffset);
		_vPos = newPos;

		VECTOR newTarget = VAdd(targetPos, _targetOffset);
		_vTarget = newTarget;
	}

	// カメラ操作を行う（右スティック）
	{
		// Y軸回転
		float sx = _vPos.x - _vTarget.x;
		float sz = _vPos.z - _vTarget.z;
		float rad = atan2(sz, sx);
		float length = sqrt(sz * sz + sx * sx);
		if(_rx > _analogMin) { rad -= 0.05f; }
		if(_rx < -_analogMin) { rad += 0.05f; }
		_vPos.x = _vTarget.x + cos(rad) * length;
		_vPos.z = _vTarget.z + sin(rad) * length;

		// Y位置
		if(_ry > _analogMin) { _vPos.y -= 5.f; }
		if(_ry < -_analogMin) { _vPos.y += 5.f; }
	}
}

void CameraManager::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
}

void CameraManager::SetTarget(std::shared_ptr<PlayerBase> target) {
	_targetObject = target;

	// カメラ位置と注視点の初期設定
	if (_targetObject) {
		const VECTOR targetPos = _targetObject->GetPos();
		_vPos = VAdd(targetPos, _posOffset);
		_vTarget = VAdd(targetPos, _targetOffset);
	}
}
