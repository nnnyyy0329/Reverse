#include "Bullet.h"

namespace BulletConstants
{

}

Bullet::Bullet() 
{
	_eCharaType = CHARA_TYPE::BULLET;
	_eBulletType = BULLET_TYPE::NONE;

	_stcBulletConfig.bulletType		= BULLET_TYPE::NONE;
	_stcBulletConfig.charType		= CHARA_TYPE::BULLET;
	_stcBulletConfig.startPos		= VGet(0.0f, 0.0f, 0.0f);
	_stcBulletConfig.dir			= VGet(0.0f, 0.0f, 0.0f);
	_stcBulletConfig.radius			= 0.0f;
	_stcBulletConfig.damage			= 0.0f;
	_stcBulletConfig.lifeTime		= 0;

	_stcEffectConfig.effectName = "";
	_stcEffectConfig.effectOffset = VGet(0.0f, 0.0f, 0.0f);
	_stcEffectConfig.soundName = "";
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
	return true;
}

bool Bullet::Process() 
{
	if(!IsBulletAlive()){ return false; }	// 弾の生存時間が残っていないならスキップ

	// 移動処理
	_vOldPos = _vPos;// 前フレームの位置を保存
	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// 生存時間の減算処理
	DecrementLifeTime();

	return true;
}

bool Bullet::Render()
{
	// 当たり判定のsphereを描画
	// 中心座標、半径、分割数、ディフューズ、スペキュラ、塗りつぶすか
	DrawSphere3D(
		_vPos, _fCollisionR, 8, GetColor(0, 255, 255), GetColor(255, 255, 255), FALSE
	);

	return true;
}

void Bullet::DebugRender()
{
	
}

void Bullet::CollisionRender() 
{

}

void Bullet::Activate(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime, CHARA_TYPE charType, BULLET_TYPE bulletType)
{
	_vPos = vStartPos;
	_vOldPos = vStartPos;// 前フレームの位置も初期化

	_vDir = VNorm(vDir);// 向きを正規化
	_fCollisionR = fRadius;
	_fMoveSpeed = fSpeed;
	_lifeTimer = lifeTime;
	_eShooterType = charType;
	_eBulletType = bulletType;

	_vMove = VScale(_vDir, _fMoveSpeed);// 移動量を設定
}

// 弾を有効化する
void Bullet::ActivateBullet(const BulletConfig& bConfig, const BulletEffectConfig& bEffectConfig)
{
	// 弾情報の設定
	SetBulletConfig(bConfig);

	// 弾の演出関連の情報設定
	SetEffectConfig(bEffectConfig);

	// 他の情報設定
	SetCoordinateConfig(bConfig);
}

// 弾情報の設定
void Bullet::SetBulletConfig(const BulletConfig& config)
{
	_stcBulletConfig = config;	// 情報の設定
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
}

// 弾の生存時間の減算処理
void Bullet::DecrementLifeTime()
{
	_lifeTimer--;	// 時間を減らす
}

// 弾の生存時間が残っているか
bool Bullet::IsBulletAlive()const
{
	// 	// 生存時間が残っているなら
	if(_lifeTimer > 0) 
	{
		return true;	// リストから削除する
	}
	
	// 弾の生存時間が残っていない
	return false;	
}
