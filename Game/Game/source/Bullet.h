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
	BULLET_TYPE bulletType;	// 弾の種類
	CHARA_TYPE shooterType;	// 発射者のキャラタイプ
	VECTOR startPos;		// 発射位置
	VECTOR dir;				// 発射方向
	float radius;			// 当たり判定の半径
	float damage;			// ダメージ量
	float speed;			// 移動速度
	int lifeTime;			// 寿命
	bool isHit = false;		// ヒットしたか(初期化しておく)
};

// 弾の演出関係の構造体
struct BulletEffectConfig
{
	// エフェクト
	std::string effectName;	// エフェクト名
	VECTOR effectOffset;	// エフェクトのオフセット

	// サウンド
	std::string soundName;	// サウンド名
};

// 弾クラス
class Bullet : public CharaBase
{
public:
	Bullet();
	virtual ~Bullet();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新	
	virtual bool Render();		// 描画

	virtual void DebugRender();		// デバッグ描画
	void CollisionRender();			// コリジョン描画

	// 弾の発射処理
	void Activate(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime, CHARA_TYPE charaType, BULLET_TYPE bulletType);

	// 弾を有効化する
	void ActivateBullet(const BulletConfig& config, const BulletEffectConfig& effectConfig);

	// 弾を有効化する(演出面の引数なし)
	void ActivateBulletSimple(const BulletConfig& config);

	// 弾情報の設定
	void SetBulletConfig(const BulletConfig& config);

	// 弾の演出関連の情報設定
	void SetEffectConfig(const BulletEffectConfig& config);

	// 位置関係の情報設定
	void SetCoordinateConfig(const BulletConfig& config);

	// ヒット時の演出処理
	void PlayEffect(const BulletEffectConfig& effectConfig);

	// エフェクト再生処理
	void PlayBulletEffect(const BulletEffectConfig& effectConfig);

	// サウンド再生処理
	void PlayBulletSound(const BulletEffectConfig& effectConfig);

	// 弾の生存時間が残っているか
	bool IsBulletAlive()const;


	/* ゲッターセッター */

	// 情報設定
	const BulletConfig& GetBulletConfig() const { return _stcBulletConfig; }

	// エフェクト設定
	const BulletEffectConfig& GetBulletEffectConfig() const { return _stcEffectConfig; }

	CHARA_TYPE GetShooterType() const { return _eShooterType; }	// 弾の発射者を取得
	BULLET_TYPE GetBulletType() const { return _eBulletType; }	// 弾の種類を取得

private:

	// 弾の移動処理
	void MoveBullet();

	// 弾の生存時間の減算処理
	void DecrementLifeTime();

protected:

	// 弾の情報構造体
	BulletConfig		_stcBulletConfig;		

	// 弾の演出関係の構造体
	BulletEffectConfig	_stcEffectConfig;	

	CHARA_TYPE _eShooterType;	// 誰が発射した弾か
	BULLET_TYPE _eBulletType;	// 弾の種類
};

