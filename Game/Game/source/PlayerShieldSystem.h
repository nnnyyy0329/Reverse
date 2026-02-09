#pragma once
#include "appframe.h"

// プレイヤーのシールドシステム
class PlayerShieldSystem
{
public:
	PlayerShieldSystem();
	virtual ~PlayerShieldSystem();

	bool Initialize();			// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

protected:

};

