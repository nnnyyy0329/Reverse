#pragma once
#include "CharaBase.h"

class Bullet : public CharaBase
{
public:
	Bullet();
	virtual ~Bullet();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新	
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ描画

	void Activate(VECTOR vStartPos, VECTOR vDir, float fSpeed, int lifeTime);// 弾を発射状態にする

protected:
	int _lifeTimer;// 弾の寿命(フレーム)
};

