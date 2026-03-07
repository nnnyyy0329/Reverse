#include "BulletManager.h"

// シングルトン用メンバ初期化
BulletManager* BulletManager::_instance = nullptr;

// インスタンス作成
void BulletManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new BulletManager();	// インスタンス作成
		_instance->Initialize();			// 初期化
	}
}

// インスタンス破棄
void BulletManager::DestroyInstance()
{
	if(_instance != nullptr)
	{
		delete _instance;		// インスタンス破棄
		_instance = nullptr;	// インスタンス初期化
	}
}

BulletManager::BulletManager() 
{

}

BulletManager::~BulletManager() 
{
	// すべての弾を削除
	ClearAllBullets();
}

void BulletManager::Initialize()
{
	// 中身の初期化
	_registerBullets.clear();
}

void BulletManager::Process()
{
	// 登録された弾の更新
	UpdateBullet();
}

void BulletManager::Render() 
{
	// 登録された弾の描画
	RenderBullet();
}

void BulletManager::DebugRender()
{
	int registeredCount = static_cast<int>(_registerBullets.size());	// 弾の数

	// 登録された弾の数の描画
	DrawFormatString(10, 250, GetColor(255, 255, 255), "Registered Bullets: %d", registeredCount);
}

void BulletManager::CollisionRender()
{
	// 登録された弾のコリジョン描画
	for(auto& bulletInfo : _registerBullets)
	{
		if(bulletInfo.bullet.expired()) { continue; }
		auto bullet = bulletInfo.bullet.lock();	// 有効チェック

		if(bullet) 
		{
			// 有効ならコリジョン描画
			bullet->CollisionRender();
		}
	}
}


/* 弾発射関連 */

// 登録された球の更新
void BulletManager::UpdateBullet()
{
	// 登録された弾の更新
	for(auto& bulletInfo : _registerBullets)
	{
		if(bulletInfo.bullet.expired()) { continue; }
		auto bullet = bulletInfo.bullet.lock();	// 有効チェック

		if(bullet)
		{
			// 有効なら更新
			bullet->Process();
		}
	}

	// 無効な弾のクリーンアップ
	CleanupInvalidBullets();
}

// 登録された弾の描画
void BulletManager::RenderBullet()
{
	// 登録された弾の描画
	for(auto& bulletInfo : _registerBullets)
	{
		if(bulletInfo.bullet.expired()) { continue; }
		auto bullet = bulletInfo.bullet.lock();	// 有効チェック

		if(bullet) 
		{
			// 有効なら描画
			bullet->Render();
		}
	}
}

// 弾の発射
std::shared_ptr<Bullet> BulletManager::Shoot(const BulletConfig& bulletConfig, const BulletEffectConfig& bEffectConfig, BULLET_OWNER_TYPE ownerType)
{
	// 弾を生成
	auto bullet = std::make_shared<Bullet>();

	// BulletConfigをコピーして所有者をキャラタイプに変換
	BulletConfig modifiedConfig = bulletConfig;
	modifiedConfig.charType = ConvertOwnerTypeToCharType(ownerType);	// 変換

	// 弾を発射
	bullet->ActivateBullet(modifiedConfig, bEffectConfig);

	// 弾を登録
	RegisterBullet(bullet, ownerType);

	return bullet;
}

// 弾の発射(演出面の引数なし版)
std::shared_ptr<Bullet> BulletManager::ShootSimple(const BulletConfig& bulletConfig, BULLET_OWNER_TYPE ownerType)
{
	// 弾を生成
	auto bullet = std::make_shared<Bullet>();

	// BulletConfigをコピーして所有者をキャラタイプに変換
	BulletConfig modifiedConfig = bulletConfig;
	modifiedConfig.charType = ConvertOwnerTypeToCharType(ownerType);	// 変換

	// 弾を発射
	bullet->ActivateBulletSimple(modifiedConfig);

	// 弾を登録
	RegisterBullet(bullet, ownerType);

	return bullet;
}

// 所有者をキャラタイプに変換
CHARA_TYPE BulletManager::ConvertOwnerTypeToCharType(BULLET_OWNER_TYPE ownerType)
{
	// BULLET_OWNER_TYPE を CHARA_TYPE に変換
	CHARA_TYPE charaType = CHARA_TYPE::NONE;

	// 所有者タイプで判別
	switch(ownerType)
	{
		case BULLET_OWNER_TYPE::BULLET_PLAYER: // 弾プレイヤー
		{
			// 弾プレイヤー
			return CHARA_TYPE::BULLET_PLAYER;
		}

		case BULLET_OWNER_TYPE::ENEMY: // 敵
		{
			// 敵
			return CHARA_TYPE::ENEMY;
		}

		default:
		{
			return CHARA_TYPE::NONE;
		}
	}
}


/* 弾管理関連 */

// 弾の登録
void BulletManager::RegisterBullet(std::shared_ptr<Bullet> bullet, BULLET_OWNER_TYPE ownerType)
{
	if(!bullet){ return; }

	// 登録情報
	BULLET_INFO info;

	info.bullet = bullet;		// 弾クラスを設定
	info.ownerType = ownerType;	// 所有者を設定

	// 登録する
	_registerBullets.push_back(info);
}

// 弾を削除
void BulletManager::RemoveBullet(std::shared_ptr<Bullet> bullet)
{
	if(!bullet){ return; }

	// 登録された弾を走査
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end(); ++bullets)
	{
		// 弾が有効なら
		if(!bullets->bullet.expired() && bullets->bullet.lock() == bullet)
		{
			// 登録された弾から削除
			_registerBullets.erase(bullets);

			break;
		}
	}
}

// 弾を所有者タイプで削除
void BulletManager::RemoveBulletByOwnerType(BULLET_OWNER_TYPE ownerType)
{
	// 登録された弾を走査
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end();)
	{
		// タイプが一致するなら
		if(bullets->ownerType == ownerType)
		{
			// 弾を削除
			bullets = _registerBullets.erase(bullets);
		}
		// タイプが一致しないなら
		else
		{
			// 次の弾を調べる
			++bullets;
		}
	}
}

// すべての弾を削除
void BulletManager::ClearAllBullets()
{
	// 削除
	_registerBullets.clear();
}

// 無効な弾の削除
void BulletManager::CleanupInvalidBullets()
{
	// 登録された弾を走査
	for(auto bullets = _registerBullets.begin(); bullets != _registerBullets.end();)
	{
		// 弾が無効なら
		if(bullets->bullet.expired())
		{
			bullets = _registerBullets.erase(bullets);
		}
		// 弾が有効なら
		else
		{
			// 次の弾を調べる
			++bullets;
		}
	}
}

// 弾が登録済みかチェック
bool BulletManager::IsBulletRegistered(std::shared_ptr<Bullet> bullet) const
{
	if(!bullet) { return false; }

	// 登録された弾を走査
	for(const auto& info : _registerBullets)
	{
		// 弾が有効なら
		if(!info.bullet.expired() && info.bullet.lock() == bullet)
		{
			// 登録済み
			return true;
		}
	}

	// 未登録
	return false;
}


/* 弾の回避関連 */

// 回避された弾を登録
void BulletManager::RegisterDodgeBullet(std::shared_ptr<Bullet> bullet)
{
	if(!bullet || IsDodgeBullet(bullet)) { return; }	// 回避済みの弾ならスキップ

	// 回避済みの弾に追加
	_dodgeBullets.push_back(bullet);
}

// 回避済み弾をクリア
void BulletManager::ClearDodgeBullets()
{
	// 削除
	_dodgeBullets.clear();
}

// 回避済みかチェック
bool BulletManager::IsDodgeBullet(std::shared_ptr<Bullet> bullet)const
{
	if(!bullet) { return false; }

	// 回避済みの弾を走査
	for(const auto& dodgedBullet : _dodgeBullets)
	{
		// 回避済みの弾が有効なら
		if(!dodgedBullet.expired() && dodgedBullet.lock() == bullet)
		{
			// 回避済みの弾だったのでスキップ
			return true;
		}
	}

	// 回避していない弾
	return false;
}


/* 弾設定関連 */

bool BulletManager::UpdateBulletConfig(std::shared_ptr<Bullet> bullet, const BulletConfig& newConfig)
{
	if(!bullet || !IsBulletRegistered(bullet)) { return false; }	// 登録済みじゃないならスキップ

	// 弾の基本情報を設定
	bullet->SetBulletConfig(newConfig);

	// 弾の位置関係の情報設定
	bullet->SetCoordinateConfig(newConfig);

	return true;
}

bool BulletManager::UpdateBulletEffectConfig(std::shared_ptr<Bullet> bullet, const BulletEffectConfig& newConfig)
{
	if(!bullet || !IsBulletRegistered(bullet)) { return false; }	// 登録済みじゃないならスキップ

	// 弾の演出情報設定
	bullet->SetEffectConfig(newConfig);

	return true;
}


/* 情報取得関連 */

// 所有者タイプで弾の取得
std::vector<std::shared_ptr<Bullet>> BulletManager::GetBulletsByOwnerType(BULLET_OWNER_TYPE ownerType)const
{
	std::vector<std::shared_ptr<Bullet>> result;	// 弾

	for(auto& info : _registerBullets)
	{
		// タイプが一致していて、弾が有効かチェック
		if(info.ownerType == ownerType && !info.bullet.expired())
		{
			auto bullet = info.bullet.lock();	// 有効かチェック

			if(bullet) 
			{
				// 弾の数を取得
				result.push_back(bullet);
			}
		}
	}

	// 数を返す
	return result;
}

// すべての弾の取得
std::vector<std::shared_ptr<Bullet>> BulletManager::GetAllBullets()const
{
	std::vector<std::shared_ptr<Bullet>> result;	// 弾

	// 弾を走査
	for(auto& info : _registerBullets)
	{
		// 弾が有効なら
		if(!info.bullet.expired())
		{
			auto bullet = info.bullet.lock();	// 有効かチェック

			if(bullet)
			{
				// 弾の数を取得
				result.push_back(bullet);
			}
		}
	}

	// 数を返す
	return result;
}

// 所有者タイプの取得
BULLET_OWNER_TYPE BulletManager::GetBulletOwnerType(std::shared_ptr<Bullet> bullet)const
{
	if(!bullet) { return BULLET_OWNER_TYPE::NONE; }

	// 弾を走査
	for(auto& info : _registerBullets)
	{
		// 弾が有効なら
		if(!info.bullet.expired() && info.bullet.lock() == bullet)
		{
			// 所有者のタイプを返す
			return info.ownerType;
		}
	}

	// 無効な弾なら所有者なし
	return BULLET_OWNER_TYPE::NONE;
}

