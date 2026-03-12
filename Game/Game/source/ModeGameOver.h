#pragma once
#include "appframe.h"

// ゲームオーバー時のモード
class ModeGameOver : public ModeBase
{
	typedef ModeBase base;

public:
	ModeGameOver();
	virtual ~ModeGameOver();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

private:
	int _menuIndex = 0;
};

