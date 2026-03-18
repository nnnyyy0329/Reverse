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
	_bUseHorrizontalOnly = false;
}

void CameraDollyAddon::Process()
{
	if (!_bIsActive) { return; }

	_fCurrentTime++;

	if (_fCurrentTime >= _fDuration)
	{
		_fCurrentTime = 0.0f;
		_bIsActive = false;
		return;
	}

	// 現在フレームのオフセットを計算
	_fCurrentOffset = CalculateCurrentOffset();
}

void CameraDollyAddon::Apply(CameraBase* camera)
{
	if (!_bIsActive || !camera) { return; }

	// カメラの前方ベクトルを取得
	VECTOR dir = camera->GetCameraDir();

	if (_bUseHorrizontalOnly)
	{
		dir.y = 0.0f;
		float len = VSize(dir);

		if (len > 0.0001f)
		{
			dir = VScale(dir, 1.0f / len);
		}
		else
		{
			dir = VGet(0.0f, 0.0f, 1.0f);
		}
	}

	// +distは引きに統一するため、前方逆向きにオフセットを適用
	VECTOR offset = VScale(dir, -_fCurrentOffset);

	// カメラ位置へオフセットを加算
	VECTOR newPos = VAdd(camera->GetPos(), offset);
	camera->SetPos(newPos);
}

void CameraDollyAddon::StartDolly(float maxDollyDist, float duration, float transitionTime)
{
	_fMaxDollyDist = maxDollyDist;
	_fDuration = duration;
	_fTransitionTime = transitionTime;

	// 往復時間が全体時間を超えないように制限
	if (_fDuration < _fTransitionTime * 2.0f)
	{
		_fTransitionTime = _fDuration * 0.5f;
	}

	_fCurrentOffset = 0.0f;
	_fCurrentTime = 0.0f;
	_bIsActive = true;
}

void CameraDollyAddon::Stop()
{
	_bIsActive = false;
	_bUseHorrizontalOnly = false;
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

float CameraDollyAddon::CalculateCurrentOffset()
{
	// 0から最大距離までのイージング計算
	if (_fCurrentTime < _fTransitionTime)
	{
		float rate = _fCurrentTime / _fTransitionTime;
		float ease = sinf(rate * DX_PI_F * 0.5f);
		return _fMaxDollyDist * ease;
	}

	// 最大距離から0までのイージング計算
	if (_fCurrentTime > _fDuration - _fTransitionTime)
	{
		float remainingTime = _fDuration - _fCurrentTime;
		float rate = remainingTime / _fTransitionTime;
		float ease = sinf(rate * DX_PI_F * 0.5f);
		return _fMaxDollyDist * ease;
	}

	// 最大距離を維持
	return _fMaxDollyDist;
}
