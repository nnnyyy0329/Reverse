#include "GameCamera.h"
#include "PlayerBase.h"

GameCamera::GameCamera()
{
	_vPos = VGet(0.0f, 0.0f, 0.0f);				// カメラの設定
	_vTarget = VGet(0.0f, 0.0f, 0.0f);			// 注視点
	_nearClip = 1.0f;							// 描画する手前の距離
	_farClip = 5000.0f;							// 描画する奥の距離
	_posOffset = VGet(0.0f, 200.0f, 500.0f);	// カメラ位置のオフセット
	_targetOffset = VGet(0.0f, 100.0f, 0.0f);	// 注視点のオフセット
}

GameCamera::~GameCamera()
{

}

bool GameCamera::Initialize()
{
	return true;
}

bool GameCamera::Terminate()
{
	return true;
}

bool GameCamera::Process()
{
	// カメラ位置の更新
	UpdateCameraPos();

	// カメラ操作
	ControlCamera();

	return true;
}

bool GameCamera::Render()
{
	return true;
}

// カメラ位置の更新
void GameCamera::UpdateCameraPos()
{
	// ターゲットオブジェクトがあれば
	if (_targetObject)
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

// カメラ操作
void GameCamera::ControlCamera()
{
	// カメラ操作を行う（右スティック）
	{
		float sx = _vPos.x - _vTarget.x;
		float sz = _vPos.z - _vTarget.z;		
		float rad = atan2(sz, sx);		
		float length = sqrt(sz * sz + sx * sx);	

		// Y軸回転
		if(_rx > _analogMin) { rad -= 0.05f; }
		if(_rx < -_analogMin) { rad += 0.05f; }

		// XZ位置更新
		_vPos.x = _vTarget.x + cos(rad) * length;
		_vPos.z = _vTarget.z + sin(rad) * length;

		// Y位置更新
		if(_ry > _analogMin) { _vPos.y -= 5.f; }
		if(_ry < -_analogMin) { _vPos.y += 5.f; }
	}
}

// ターゲットを設定する関数
void GameCamera::SetTarget(std::shared_ptr<PlayerBase> target) 
{
	// ターゲットオブジェクトを設定
	_targetObject = target;

	// カメラ位置と注視点の初期設定
	if(_targetObject) 
	{
		const VECTOR targetPos = _targetObject->GetPos();	// ターゲットの位置を取得
		_vPos = VAdd(targetPos, _posOffset);				// カメラ位置
		_vTarget = VAdd(targetPos, _targetOffset);			// 注視点
	}
}

// カメラの設定
void GameCamera::SetUp()
{
	// カメラの位置と注視点を設定
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);

	// カメラからどれだけ離れた距離からどこまで描画するかを設定
	SetCameraNearFar(_nearClip, _farClip);
}
