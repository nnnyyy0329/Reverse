#pragma once
#include "appframe.h"

class ModeEndingVideo : public ModeBase
{
public:
	typedef ModeBase base;

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	int _movieGraph = -1;
	bool _started = false;
	bool _finished = false;
	int _frameCount = 0;
};