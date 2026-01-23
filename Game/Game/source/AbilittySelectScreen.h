#pragma once
#include "appframe.h"

// アビリティ選択画面クラス
class AbilittySelectScreen
{
public:
	AbilittySelectScreen();
	~AbilittySelectScreen();
	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

protected:
	// 表示用ハンドル
	int _iHandle1;
	int _iHandle2;
	int _iHandle3;

	int _drawX;
	int _drawY;
};

