#pragma once
#include "appframe.h"

class ModeTitle : public ModeBase 
{
	typedef ModeBase base;
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	bool _bIsAddLoading = false;
};
