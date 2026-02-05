#pragma once
#include "appframe.h"

// シールド基底クラス
class ShieldBase
{
public:
	ShieldBase();
	virtual ~ShieldBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了	
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

protected:


};

