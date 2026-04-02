/*****************************************
* file   BulletManager.h
* brief  弾管理クラス
* author 成田 悠真
* date   2026/01/16
******************************************/

#pragma once
#include "Bullet.h"

/// @brief 弾の所有者タイプ列挙型
enum class BULLET_OWNER_TYPE
{
	NONE,
	BULLET_PLAYER,	// 弾プレイヤー
	ENEMY,			// 敵
	_EOT_,
};

/// @brief 弾情報構造体
struct BULLET_INFO
{
	std::shared_ptr<Bullet> bullet;	// 弾オブジェクト
	BULLET_OWNER_TYPE ownerType;	// 所有者タイプ
};

/// @brief 弾管理クラス
class BulletManager
{
public:

	/* シングルトン関数 */

	/// @brief インスタンス取得関数
	static BulletManager* GetInstance()
	{
		static BulletManager instance;	// 静的ローカル変数としてインスタンスを生成
		return &instance;				// インスタンスのアドレスを返す
	}

	/// @brief インスタンス生成関数
	static void CreateInstance();	

	/// @brief インスタンス破棄関数
	static void DestroyInstance();	



	/* 基本関数 */

	void Initialize();
	void Process();
	void Render();

	
	/* デバッグ関数 */

	/// @brief デバッグ描画関数
	void DebugRender();		

	/// @brief コリジョン描画関数
	void CollisionRender();	

	/* 弾発射関連 */

	/// @brief 登録された弾の更新関数
	void UpdateBullet();

	/// @brief 登録された弾の描画関数
	void RenderBullet();

	/// @brief 弾の発射関数
	///
	/// @param bulletConfig 弾の情報構造体
	/// @param bulletEffectConfig 弾の演出関連の構造体
	/// @param ownerType 弾の所有者タイプ
	/// 
	/// @return 発射された弾の共有ポインタ
	std::shared_ptr<Bullet>Shoot(const BulletConfig& bulletConfig, const BulletEffectConfig& bulletEffectConfig, BULLET_OWNER_TYPE ownerType);

	/// @brief 弾の発射関数(演出面の引数なし版)
	///
	/// @param bulletConfig 弾の情報構造体
	/// @param ownerType 弾の所有者タイプ
	std::shared_ptr<Bullet>ShootSimple(const BulletConfig& bulletConfig, BULLET_OWNER_TYPE ownerType);

	/// @brief 所有者タイプをキャラタイプに変換する関数
	///
	/// @param ownerType 変換する所有者タイプ
	/// 
	/// @return 変換されたキャラタイプ
	CHARA_TYPE ConvertOwnerTypeToCharType(BULLET_OWNER_TYPE ownerType);


	/* 弾管理関連 */

	/// @brief 弾を登録する関数
	///
	/// @param bullet 登録する弾の共有ポインタ
	/// @param ownerType 登録する弾の所有者タイプ
	void RegisterBullet(std::shared_ptr<Bullet>bullet, BULLET_OWNER_TYPE ownerType);

	/// @brief 弾回避後の処理
	void ProcessEvadeBullet();	

	/// @brief 弾を削除する関数
	///
	/// @param bullet 削除する弾の共有ポインタ
	void RemoveBullet(std::shared_ptr<Bullet> bullet);	

	/// @brief 所有者タイプで弾を削除する関数
	///
	/// @param ownerType 削除する弾の所有者タイプ
	void RemoveBulletByOwnerType(BULLET_OWNER_TYPE ownerType);

	/// @brief すべての弾を削除する関数
	///
	/// @param bullets 削除する弾の共有ポインタのベクター
	void ClearAllBullets(std::vector<std::shared_ptr<Bullet>> bullets);

	/// @brief 無効な弾を削除する関数
	void CleanupInvalidBullets();

	/// @brief 弾が登録済みかチェックする関数
	///
	/// @param bullet チェックする弾の共有ポインタ
	///	
	/// @return 登録済みならtrue、そうでないならfalse
	bool IsBulletRegistered(std::shared_ptr<Bullet> bullet) const;


	/* 弾の回避関連 */

	/// @brief 回避された弾を登録する関数
	///
	/// @param bullet 登録する回避された弾の共有ポインタのベクター
	void RegisterDodgeBullet(std::vector<std::shared_ptr<Bullet>> bullet);

	/// @brief 回避された弾をクリアする関数
	void ClearDodgeBullets();									

	/// @brief 指定した弾が回避された弾かチェックする関数
	///
	/// @param bullet チェックする弾の共有ポインタのベクター
	/// 
	/// @return 回避された弾ならtrue、そうでないならfalse
	bool IsDodgeBullet(std::vector<std::shared_ptr<Bullet>> bullet)const;


	/* 弾設定関連 */

	/// @brief 登録済み弾の情報を更新する関数
	///
	/// @param bullet 更新する弾の共有ポインタ
	/// @param newConfig 更新する新しい弾の情報構造体
	/// 
	/// @return 更新成功ならtrue、そうでないならfalse
	bool UpdateBulletConfig(std::shared_ptr<Bullet> bullet, const BulletConfig& newConfig);

	/// @brief 登録済み弾のエフェクト設定を更新する関数
	///
	/// @param bullet 更新する弾の共有ポインタ
	/// @param newConfig 更新する新しい弾の演出関連の構造体
	/// 
	/// @return 更新成功ならtrue、そうでないならfalse
	bool UpdateBulletEffectConfig(std::shared_ptr<Bullet> bullet, const BulletEffectConfig& newConfig);


	/* 情報取得関連 */

	/// @brief 所有者タイプで弾を取得する関数
	///
	/// @param ownerType 取得する弾の所有者タイプ
	/// 
	/// @return 取得された弾の共有ポインタのベクター
	std::vector<std::shared_ptr<Bullet>>GetBulletsByOwnerType(BULLET_OWNER_TYPE ownerType)const;

	/// @brief 登録されたすべての弾を取得する関数
	///
	/// @return 登録されたすべての弾の共有ポインタのベクター
	std::vector<std::shared_ptr<Bullet>>GetAllBullets()const;

	/// @brief 弾の所有者タイプを取得する関数
	///
	/// @param bullet 取得する弾の共有ポインタ
	/// 
	/// @return 取得された弾の所有者タイプ
	BULLET_OWNER_TYPE GetBulletOwnerType(std::shared_ptr<Bullet> bullet)const;

	/// @brief 登録された弾の数を取得する関数
	///
	/// @return 登録された弾の数
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

