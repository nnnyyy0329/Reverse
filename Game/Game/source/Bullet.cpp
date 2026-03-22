#include "Bullet.h"

namespace BulletConstants
{

}

Bullet::Bullet() 
{
	_stcBulletConfig.bulletType		= BULLET_TYPE::NONE;
	_stcBulletConfig.shooterType	= CHARA_TYPE::BULLET;
	_stcBulletConfig.startPos		= VGet(0.0f, 0.0f, 0.0f);
	_stcBulletConfig.dir			= VGet(0.0f, 0.0f, 0.0f);
	_stcBulletConfig.radius			= 0.0f;
	_stcBulletConfig.damage			= 0.0f;
	_stcBulletConfig.lifeTime		= 0;

	_stcEffectConfig.effectName = "";
	_stcEffectConfig.effectOffset = VGet(0.0f, 0.0f, 0.0f);
	_stcEffectConfig.soundName = "";

	_eCharaType = CHARA_TYPE::BULLET;
	_eBulletType = BULLET_TYPE::NONE;

	_effectHandle = -1;
}

Bullet::~Bullet() 
{

}

bool Bullet::Initialize() 
{
	return true;
}

bool Bullet::Terminate() 
{
	// エフェクトの停止
	EffectServer::GetInstance()->Stop(_effectHandle);

	return true;
}

bool Bullet::Process() 
{
	if(!IsBulletAlive()){ return false; }	// 弾の生存時間が残っていないならスキップ

	// 弾の移動処理
	MoveBullet();

	// 弾のエフェクト位置更新
	UpdateBulletEffectPos();

	// 生存時間の減算処理
	DecrementLifeTime();

	return true;
}

bool Bullet::Render()
{
	// 当たり判定の描画
	CollisionRender();

	return true;
}

void Bullet::DebugRender()
{
	// 当たり判定の描画
	CollisionRender();
}

// 当たり判定の描画
void Bullet::CollisionRender() 
{
	// 当たり判定のsphereを描画
	DrawSphere3D
	(
		_vPos, 
		_stcBulletConfig.radius,
		8, 
		GetColor(0, 255, 255), 
		GetColor(255, 255, 255),
		FALSE
	);
}

// 弾を有効化する
void Bullet::ActivateBullet(const BulletConfig& bulletConfig, const BulletEffectConfig& bulletEffectConfig)
{
	// 弾情報の設定
	SetBulletConfig(bulletConfig);

	// 弾の演出関連の情報設定
	SetEffectConfig(bulletEffectConfig);

	// 他の情報設定
	SetCoordinateConfig(bulletConfig);

	// エフェクトを再生
	PlayBulletEffect(bulletEffectConfig);
}

// 弾を有効化する(演出面の引数なし)
void Bullet::ActivateBulletSimple(const BulletConfig& config)
{
	// 弾情報の設定
	SetBulletConfig(config);

	// 他の情報設定
	SetCoordinateConfig(config);
}

// 弾のエフェクト位置更新
void Bullet::UpdateBulletEffectPos()
{
	// エフェクト名が空の場合はスキップ
	if(_stcEffectConfig.effectName.empty())
	{
		return;
	}

	// エフェクト再生位置を計算
	VECTOR effectPos = VAdd(_vPos, _stcEffectConfig.effectOffset);

	// エフェクトサーバーに位置を通知
	EffectServer::GetInstance()->SetPos(_effectHandle, effectPos);
}

// 弾情報の設定
void Bullet::SetBulletConfig(const BulletConfig& config)
{
	_stcBulletConfig = config;	// 情報の設定

	// 必要な情報を受け取る
	_eBulletType = config.bulletType;	// 弾のタイプ
	_eShooterType = config.shooterType;	// 所有者タイプ
	_fCollisionR = config.radius;		// 弾半径
}

// 弾の演出関連の情報設定
void Bullet::SetEffectConfig(const BulletEffectConfig& config)
{
	_stcEffectConfig = config;	// 情報設定
}

// 他の情報設定
void Bullet::SetCoordinateConfig(const BulletConfig& config)
{
	_vPos		= config.startPos;				// 位置
	_vOldPos	= config.startPos;				// 1フレーム前の位置
	_vDir		= VNorm(config.dir);			// 向きの正規化
	_vMove		= VScale(_vDir, config.speed);	// 移動量の計算
	_fMoveSpeed = config.speed;
}

// ヒット時の演出処理
void Bullet::PlayEffect(const BulletEffectConfig& config)
{
	// エフェクト再生処理
	PlayBulletEffect(config);

	// サウンド再生処理
	PlayBulletSound(config);
}
// エフェクト再生処理
void Bullet::PlayBulletEffect(const BulletEffectConfig& config)
{
	// エフェクト名が空でない場合のみ
	if(!config.effectName.empty())
	{
		// エフェクト再生位置を計算
		VECTOR effectPos = VAdd(_vPos, config.effectOffset);

		// エフェクト再生
		_effectHandle = EffectServer::GetInstance()->Play(config.effectName, effectPos);
	}
}
	
// サウンド再生処理
void Bullet::PlayBulletSound(const BulletEffectConfig& config)
{
	// サウンド名が空でない場合のみ
	if(!config.soundName.empty())
	{
		// サウンド再生
		SoundServer::GetInstance()->Play(config.soundName, DX_PLAYTYPE_BACK);
	}
}

// 弾の移動処理
void Bullet::MoveBullet()
{
	// 前フレームの位置を保存
	_vOldPos = _vPos;

	// 位置更新
	_vPos = VAdd(_vPos, _vMove);
}

// 弾の生存時間の減算処理
void Bullet::DecrementLifeTime()
{
	_stcBulletConfig.lifeTime--;	// 時間を減らす
}

// 弾の生存時間が残っているか
bool Bullet::IsBulletAlive()const
{
	// 	// 生存時間が残っているなら
	if(_stcBulletConfig.lifeTime > 0)
	{
		return true;	// リストから削除する
	}
	
	// 弾の生存時間が残っていない
	return false;	
}
