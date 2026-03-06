#pragma once
#include "Bullet.h"

// 弾の所有者タイプ
enum class BULLET_OWNER_TYPE
{
	NONE,
	BULLET_PLAYER,	// 弾プレイヤー
	ENEMY,			// 敵
	_EOT_,
};

// 弾情報構造体
struct BULLET_INFO
{
	std::weak_ptr<Bullet> bullet;	// 弾オブジェクト
	BULLET_OWNER_TYPE ownerType;	// 所有者タイプ
};

class BulletManager
{
public:

	// シングルトンアクセス
	static BulletManager* GetInstance()
	{
		static BulletManager instance;
		return &instance;
	}
	static void CreateInstance();	// インスタンス作成
	static void DestroyInstance();	// インスタンス破棄

	void Initialize();
	void Process();
	void Render();
	void DebugRender();		// デバッグ情報描画
	void CollisionRender();	// コリジョン描画

	/* 弾発射関連 */

	// 登録された弾の更新
	void UpdateBullet();

	// 登録された弾の描画
	void RenderBullet();

	// 弾の発射
	std::shared_ptr<Bullet>Shoot(const BulletConfig& bConfig, const BulletEffectConfig& bEffectConfig, BULLET_OWNER_TYPE ownerType);

	// 所有者をキャラタイプに変換
	CHARA_TYPE ConvertOwnerTypeToCharType(BULLET_OWNER_TYPE ownerType);


	/* 弾管理関連 */

	// 弾の登録
	void RegisterBullet(std::shared_ptr<Bullet>bullet, BULLET_OWNER_TYPE ownerType);

	// 弾を削除
	void RemoveBullet(std::shared_ptr<Bullet> bullet);	

	// 弾を所有者タイプで削除
	void RemoveBulletByOwnerType(BULLET_OWNER_TYPE ownerType);

	// すべての弾を削除
	void ClearAllBullets();

	// 無効な弾の削除
	void CleanupInvalidBullets();	

	// 弾が登録済みかチェック
	bool IsBulletRegistered(std::shared_ptr<Bullet> bullet) const;


	/* 弾の回避関連 */

	// 回避された弾を登録
	void RegisterDodgeBullet(std::shared_ptr<Bullet> bullet);	

	// 回避済み弾をクリア
	void ClearDodgeBullets();									

	// 回避済みかチェック
	bool IsDodgeBullet(std::shared_ptr<Bullet> bullet)const;	


	/* 弾設定関連 */

	// 登録済み弾の情報を更新
	bool UpdateBulletConfig(std::shared_ptr<Bullet> bullet, const BulletConfig& newConfig);

	// 登録済み弾のエフェクト設定を更新
	bool UpdateBulletEffectConfig(std::shared_ptr<Bullet> bullet, const BulletEffectConfig& newConfig);


	/* 情報取得関連 */

	// 所有者タイプで弾の取得
	std::vector<std::shared_ptr<Bullet>>GetBulletsByOwnerType(BULLET_OWNER_TYPE ownerType)const;

	// 弾の取得
	std::vector<std::shared_ptr<Bullet>>GetAllBullets()const;

	// 所有者タイプの取得
	BULLET_OWNER_TYPE GetBulletOwnerType(std::shared_ptr<Bullet> bullet)const;

	// 情報取得
	int GetBulletCount()const{ return static_cast<int>(_registerBullets.size()); }

private:

	// シングルトン用コンストラクタ、デストラクタ
	BulletManager();
	virtual ~BulletManager();

	// シングルトン用メンバ
	static BulletManager* _instance;

protected:

	// 登録された弾のリスト
	std::vector<BULLET_INFO>_registerBullets;

	// 回避された弾リスト
	std::vector<std::weak_ptr<Bullet>>_dodgeBullets;	
};

