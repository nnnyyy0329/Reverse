/*****************************************
* file   Bullet.h
* brief  弾クラス
* author 成田 悠真
* date   2026/01/15
******************************************/

#pragma once
#include "CharaBase.h"

/// @brief 弾の種類を管理するための列挙型
enum class BULLET_TYPE
{
	NONE,
	NORMAL,		///< 通常弾
	PIERCING,	///< 貫通弾
	_EOT_,
};

/// @brief 弾の情報構造体
struct BulletConfig
{
	BULLET_TYPE bulletType;	// 弾の種類
	CHARA_TYPE shooterType;	// 発射者のキャラタイプ
	VECTOR startPos;		// 発射位置
	VECTOR dir;				// 発射方向
	float radius;			// 当たり判定の半径
	float damage;			// ダメージ量
	float speed;			// 移動速度
	float lifeTime;			// 寿命
	bool isHit = false;		// ヒットしたか(初期化しておく)
};

/// @brief 弾の演出関係の構造体
struct BulletEffectConfig
{
	// エフェクト
	std::string effectName;	// エフェクト名
	VECTOR effectOffset;	// エフェクトのオフセット

	// サウンド
	std::string soundName;	// サウンド名
};

/// @brief 弾クラス
class Bullet : public CharaBase
{
public:

	Bullet();
	virtual ~Bullet();


	/* 基本関数 */

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新	
	virtual bool Render();		// 描画


	/* デバッグ関数 */
	
	/// @brief デバッグ描画関数
	virtual void DebugRender();		
	
	/// @brief コリジョン描画関数
	void CollisionRender();	


	/* 弾の処理関係関数 */

	/// @brief 弾を有効化する関数
	///
	/// @param config 弾の情報構造体
	/// @param effectConfig 弾の演出関係の構造体
	void ActivateBullet(const BulletConfig& config, const BulletEffectConfig& effectConfig);

	/// @brief 弾を有効化する関数(演出面の引数なし)
	///
	/// @param config 弾の情報構造体
	void ActivateBulletSimple(const BulletConfig& config);

	/// @brief 弾のエフェクト位置更新関数
	void UpdateBulletEffectPos();

	/// @brief 弾の情報設定関数
	///
	/// @param config 弾の情報構造体
	void SetBulletConfig(const BulletConfig& config);

	/// @brief 弾の演出関連の情報設定関数
	///
	/// @param config 弾の演出関連の構造体
	void SetEffectConfig(const BulletEffectConfig& config);

	/// @brief 弾の位置関係の情報設定関数
	///
	/// @param config 弾の情報構造体
	void SetCoordinateConfig(const BulletConfig& config);

	/// @brief ヒット時の演出処理関数
	///
	/// @param effectConfig 弾の演出関連の構造体
	void PlayEffect(const BulletEffectConfig& effectConfig);

	/// @brief 弾のエフェクト再生処理関数
	///
	/// @param effectConfig 弾の演出関連の構造体
	void PlayBulletEffect(const BulletEffectConfig& effectConfig);

	/// @brief 弾のサウンド再生処理関数
	///
	/// @param effectConfig 弾の演出関連の構造体
	void PlayBulletSound(const BulletEffectConfig& effectConfig);

	/// @brief 弾の生存時間が残っているかをチェックする関数
	///
	/// @return 生存時間が残っている場合はtrue、そうでない場合はfalse
	bool IsBulletAlive()const;


	/* ゲッターセッター */

	/// @brief 弾の情報構造体を取得する関数
	///
	/// @return 弾の情報構造体
	const BulletConfig& GetBulletConfig() const { return _stcBulletConfig; }

	/// @brief 弾の演出関連の構造体を取得する関数
	///
	/// @return 弾の演出関連の構造体
	const BulletEffectConfig& GetBulletEffectConfig() const { return _stcEffectConfig; }

	/// @brief 弾の発射者を取得する関数
	///
	/// @return 弾の発射者のキャラタイプ
	CHARA_TYPE GetShooterType() const { return _eShooterType; }	
	
	/// @brief 弾の種類を取得する関数
	///
	/// @return 弾の種類
	BULLET_TYPE GetBulletType() const { return _eBulletType; }	

private:
	
	/// @brief 弾の移動処理関数
	void MoveBullet();

	/// @brief 弾の生存時間を減算する関数
	void DecrementLifeTime();

protected:

	// 弾の情報構造体
	BulletConfig		_stcBulletConfig;		

	// 弾の演出関係の構造体
	BulletEffectConfig	_stcEffectConfig;	

	CHARA_TYPE _eShooterType;	// 誰が発射した弾か
	BULLET_TYPE _eBulletType;	// 弾の種類

	int _effectHandle;
};

