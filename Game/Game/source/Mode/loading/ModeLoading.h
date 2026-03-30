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
	bool _bIsAddScenario = false;

	bool _isLoadStarted = false;
	bool _hasFirstRender = false;

	int _frameCount = 0;
	float _displayProgress = 0.0f;

	int _seHandle;
};

