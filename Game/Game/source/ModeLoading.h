#pragma once
#include "appframe.h"

class ModeLoading : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

private:
	bool _bIsAddGame = false;
	int _frameCount = 0;
};

