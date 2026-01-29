#pragma once
#include "CharaBase.h"
class Item :
    public CharaBase
{
public:
    Item();
	virtual ~Item();


	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新	
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ描画
	virtual void CollisionRender(); // コリジョン描画

	void CreateItems(VECTOR potision ,float height ,float itemRadius);
};

