#pragma once
#include "appframe.h"

class AbilitySelectScreen
{
public:
	AbilitySelectScreen();
	~AbilitySelectScreen();
	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

protected:
	// 表示用ハンドル
	int _iHandle1;
	int _iHandle2;
	int _iHandle3;
};

