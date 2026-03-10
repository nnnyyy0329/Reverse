
#include "ModeBase.h"


ModeBase::ModeBase() 
{
	_szName = "";
	_uid = 1;
	_layer = 0;

	_cntMode = 0;
	_tmMode = 0;
	_tmStep = 0;
	_tmModeBase = 0;
	_tmPauseBase = 0;
	_tmPauseStep = 0;
	_tmOldFrame = 0;

	SetCallPerFrame(1);
	SetCallOfCount(1);
}

ModeBase::~ModeBase() 
{
}


// ModeServerに接続時、Process()の前に一度だけ呼ばれる
bool	ModeBase::Initialize()
{

	return true;
}

// ModeServerから削除される際、一度だけ呼ばれる
bool	ModeBase::Terminate()
{

	return true;
}


// --------------------------------------------------------------------------
/// @brief 毎フレーム呼ばれる。処理部を記述
// --------------------------------------------------------------------------
bool	ModeBase::Process()
{
	return	true;
}

// --------------------------------------------------------------------------
/// @brief 毎フレーム呼ばれる。描画部を記述
// --------------------------------------------------------------------------
bool	ModeBase::Render()
{
	return	true;
}

// 時間経過をさせる
void ModeBase::StepTime(unsigned long tmNow)
{
	// 時間経過処理
	if (_cntMode == 0) {
		_tmMode = 0;
		_tmStep = 0;
		_tmModeBase = tmNow;
		_tmPauseBase = 0;
		_tmPauseStep = 0;
	}
	else
	{
		_tmMode = tmNow - _tmModeBase + _tmPauseStep;
		_tmStep = tmNow - _tmOldFrame;
	}
	_tmOldFrame = tmNow;
}

// カウントを進める
void ModeBase::StepCount()
{
	_cntMode++;
}

namespace
{
	inline int ClampAlpha(int a)
	{
		if(a < 0) return 0;
		if(a > 255) return 255;
		return a;
	}
}

void ModeBase::StartFade(int inFrames, int holdFrames, int outFrames)
{
	_fadeInFrames = inFrames;
	_fadeHoldFrames = holdFrames;
	_fadeOutFrames = outFrames;
	_fadeFrame = 0;
	_fadeActive = true;
}

void ModeBase::AdvanceFade()
{
	if(!_fadeActive) return;
	++_fadeFrame;
}

int ModeBase::GetFadeAlpha() const
{
	if(!_fadeActive) return 255;

	int frame = _fadeFrame;
	if(frame < _fadeInFrames)
	{
		const float t = static_cast<float>(frame) / static_cast<float>(_fadeInFrames);
		return ClampAlpha(static_cast<int>(255.0f * t));
	}

	frame -= _fadeInFrames;
	if(frame < _fadeHoldFrames)
	{
		return 255;
	}

	frame -= _fadeHoldFrames;
	if(frame < _fadeOutFrames)
	{
		const float t = static_cast<float>(frame) / static_cast<float>(_fadeOutFrames);
		return ClampAlpha(static_cast<int>(255.0f * (1.0f - t)));
	}

	return 0;
}

bool ModeBase::IsFadeActive() const
{
	return _fadeActive;
}

bool ModeBase::IsFadeFinished() const
{
	if(!_fadeActive) return false;
	const int total = _fadeInFrames + _fadeHoldFrames + _fadeOutFrames;
	return (_fadeFrame >= total);
}