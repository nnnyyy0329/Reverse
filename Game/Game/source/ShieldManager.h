/*****************************************
* file   ShieldBase.h
* brief  シールド管理クラス
* author 成田 悠真
* date   2026/02/05
******************************************/

#pragma once
#include "appframe.h"

/// @brief シールド管理クラス
class ShieldManager
{
public:

	ShieldManager();
	~ShieldManager();


	/* 基本関数 */

	bool Initialize();	// 初期化
	bool Terminate();	// 終了	
	bool Process();		// 更新
	bool Render();		// 描画

protected:


};

