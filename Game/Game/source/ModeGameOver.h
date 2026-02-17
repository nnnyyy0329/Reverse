#pragma once
#include "appframe.h"

class ModeGame;

// ゲームオーバー時のモード
class ModeGameOver : public ModeBase
{
	typedef ModeBase base;

public:
	ModeGameOver(ModeGame* modeGame);
	virtual ~ModeGameOver();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

private:
	ModeGame* _modeGame;// 参照するModeGame
};

