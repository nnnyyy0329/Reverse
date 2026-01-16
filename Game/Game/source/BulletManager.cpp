#include "BulletManager.h"

BulletManager::BulletManager() {
}

BulletManager::~BulletManager() {
	_bullets.clear();// リストのクリア
}

void BulletManager::Initialize() {// 弾の全削除
	_bullets.clear();
}

void BulletManager::Process() {
	// 全ての弾を更新
	for (auto it = _bullets.begin(); it != _bullets.end(); ) {
		auto& bullet = *it;
		if (!bullet->Process()) {
			// 寿命切れなどでfalseが返ってきたら削除
			it = _bullets.erase(it);
		}
		else {
			++it;
		}
	}
}

void BulletManager::Render() {
	// 全ての弾を描画
	for (const auto& bullet : _bullets) {
		bullet->Render();
	}
}

void BulletManager::Shoot(VECTOR startPos, VECTOR dir, float radius, float speed, int lifeTime, CHARA_TYPE type) {
	auto bullet = std::make_shared<Bullet>();
	bullet->Activate(startPos, dir, radius, speed, lifeTime, type);// 発射設定
	_bullets.push_back(bullet);// リストに追加
}

void BulletManager::RemoveBullet(std::shared_ptr<Bullet> bullet) {
	_bullets.remove(bullet);// 特定の弾の削除
}


