#pragma once
#include "Bullet.h"

class BulletManager
{
public:
	BulletManager();
	virtual ~BulletManager();

	void Initialize();
	void Process();
	void Render();

	const std::list<std::shared_ptr<Bullet>>& GetBullets() const { return _bullets; } // 弾のリスト取得

	// 弾の発射リクエスト
	void Shoot(VECTOR startPos, VECTOR dir, float radius, float speed, int lifeTime, CHARA_TYPE type);

	void RemoveBullet(std::shared_ptr<Bullet> bullet);// 当たった弾を削除

	void ClearAllBullets() { _bullets.clear(); } // すべての弾を削除

private:
	std::list<std::shared_ptr<Bullet>> _bullets; // 弾のリスト
};

