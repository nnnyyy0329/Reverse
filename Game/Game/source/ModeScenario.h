#pragma once
#include "appframe.h"

class ModeScenario :public ModeBase
{
public:
	typedef ModeBase base;
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	int _textHandles[2] = { -1, -1 };
	int _pageIndex = 0;
	int _frameCount = 0;
	bool _bNext = false;
};


