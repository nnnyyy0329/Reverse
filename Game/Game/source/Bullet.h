#pragma once
#include "CharaBase.h"

// 弾の種類を管理するための列挙型
enum class BULLET_TYPE
{
	NONE,
	NORMAL,		// 通常弾
	PIERCING,	// 貫通弾
	_EOT_,
};

// 弾の情報構造体
struct BulletConfig
{
	VECTOR vStartPos;		// 発射位置
	VECTOR vDir;			// 発射方向
	float fRadius;			// 当たり判定の半径
	float fSpeed;			// 移動速度
	int lifeTime;			// 寿命
	CHARA_TYPE charaType;	// 発射者のキャラタイプ
	BULLET_TYPE bulletType;	// 弾の種類
};

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
	virtual void CollisionRender(); // コリジョン描画

	// 弾の発射処理
	void Activate(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime, CHARA_TYPE charaType, BULLET_TYPE bulletType);
	void Activate(const BulletConfig& config);

	CHARA_TYPE GetShooterType() const { return _eShooterType; }// 弾の発射者を取得

protected:
	BulletConfig _config;// 弾の情報構造体

	int _lifeTimer;// 弾の寿命(フレーム)
	CHARA_TYPE _eShooterType;// 誰が発射した弾か
	BULLET_TYPE _eBulletType;// 弾の種類
};

