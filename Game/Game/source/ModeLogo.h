#pragma once
#include "appframe.h"


class ModeLogo : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	int _logoHandle = -1;
	int _frameCount = 0;
	bool _bNext = false;
};