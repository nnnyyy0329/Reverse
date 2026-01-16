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

	// 弾の発射処理
	void Activate(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime, CHARA_TYPE type);

	CHARA_TYPE GetShooterType() const { return _eShooterType; }// 弾の発射者を取得

protected:
	int _lifeTimer;// 弾の寿命(フレーム)
	CHARA_TYPE _eShooterType;// 誰が発射した弾か
};

