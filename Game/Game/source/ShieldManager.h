#pragma once
#include "appframe.h"

// シールド管理クラス
class ShieldManager
{
public:
	ShieldManager();
	~ShieldManager();
	bool Initialize();	// 初期化
	bool Terminate();	// 終了	
	bool Process();		// 更新
	bool Render();		// 描画

protected:


};

