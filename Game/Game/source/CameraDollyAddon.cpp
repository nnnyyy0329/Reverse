#include "CameraDollyAddon.h"
#include "CameraBase.h"

CameraDollyAddon::CameraDollyAddon()
{
	_fMaxDollyDist = 0.0f;
	_fCurrentOffset = 0.0f;
	_fDuration = 0.0f;
	_fCurrentTime = 0.0f;
	_fTransitionTime = 0.0f;
	_bIsActive = false;
}

void CameraDollyAddon::Process()
{
	if (!_bIsActive) { return; }

	_fCurrentTime++;

	if (_fCurrentTime >= _fDuration)
	{
		_bIsActive = false;
		return;
	}

	// イージング処理
	// 徐々に最大距離まで移動する
	if (_fCurrentTime < _fTransitionTime)
	{
		float rate = _fCurrentTime / _fTransitionTime;
		float ease = sinf(rate * DX_PI_F / 2.0f);// sinカーブでイージング
		_fCurrentOffset = _fMaxDollyDist * ease;
	}
	// 徐々に元の位置に戻る
	else if (_fCurrentTime > _fDuration - _fTransitionTime)
	{
		float remainingTime = _fDuration - _fCurrentTime;
		float rate = remainingTime / _fTransitionTime;
		float ease = sinf(rate * DX_PI_F / 2.0f);// sinカーブでイージング
		_fCurrentOffset = _fMaxDollyDist * ease;
	}
	// 最大距離を維持
	else
	{
		_fCurrentOffset = _fMaxDollyDist;
	}
}

void CameraDollyAddon::Apply(CameraBase* camera)
{
	if (!_bIsActive || !camera) { return; }

	// カメラの前方ベクトルを取得
	VECTOR dir = camera->GetCameraDir();

	// カメラを引くため、前方ベクトルの逆にオフセットをかける
	VECTOR offset = VScale(dir, -_fCurrentOffset);

	VECTOR newPos = VAdd(camera->GetPos(), offset);
	camera->SetPos(newPos);
}

void CameraDollyAddon::StartDolly(float maxDollyDist, float duration, float transitionTime)
{
	_fMaxDollyDist = maxDollyDist;
	_fDuration = duration;
	_fTransitionTime = transitionTime;

	// 持続時間が往復時間より短い場合は、移行時間の値を調整
	if (_fDuration < _fTransitionTime * 2.0f)
	{
		_fTransitionTime = _fDuration / 2.0f;
	}

	_fCurrentOffset = 0.0f;
	_fCurrentTime = 0.0f;
	_bIsActive = true;
}

void CameraDollyAddon::Stop()
{
	_bIsActive = false;
}

void CameraDollyAddon::CancelSmoothly()
{
	if (!_bIsActive) { return; }

	// 残りの持続時間を移行時間に上書きする
	if (_fDuration - _fCurrentTime > _fTransitionTime)
	{
		_fDuration = _fCurrentTime + _fTransitionTime;
	}
}
