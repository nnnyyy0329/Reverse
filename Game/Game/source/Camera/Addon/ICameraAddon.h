#pragma once

class CameraBase;

class ICameraAddon
{
public:
	virtual ~ICameraAddon() = default;

	virtual void Process() = 0;

	// カメラに効果を適用する
	virtual void Apply(CameraBase* camera) = 0;

	// 効果が終了したかどうか
	virtual bool IsFinished() = 0;

};

