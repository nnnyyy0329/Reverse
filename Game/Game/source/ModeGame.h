#pragma once
#include "appframe.h"

//いったんこれ
class SurfacePlayer;
class StageBase;

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

protected:

	//いったんこれ
	std::shared_ptr<SurfacePlayer> _surfacePlayer;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase> _stage;// ステージ
};
