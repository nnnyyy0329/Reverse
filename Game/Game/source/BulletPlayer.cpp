#include "BulletPlayer.h"
#include "BulletManager.h"

namespace bulletConfig
{
	const VECTOR START_POS_OFFSET = VGet(0, 70, 0);
	const float RADIUS = 50.0f;
	const float SPEED = 15.0f;
	const float LIFE_TIME = 120.0f;
}

BulletPlayer::BulletPlayer()
{
	_eCharaType = CHARA_TYPE::BULLET_PLAYER;
}

BulletPlayer::~BulletPlayer()
{

}

bool BulletPlayer::Initialize()
{
	PlayerBase::Initialize();

	return true;
}

bool BulletPlayer::Terminate()
{
	return true;
}

bool BulletPlayer::Process()
{
	PlayerBase::Process();

	// 入力確認
	ShootByInput();

	return true;
}

bool BulletPlayer::Render()
{
	PlayerBase::Render();

	return true;
}

// デバッグ描画
void BulletPlayer::DebugRender()
{
	PlayerBase::DebugRender();
}

// 被ダメージ処理
void BulletPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType)
{
	PlayerBase::ApplyDamage(fDamage, eType);
}

// 弾プレイヤーの情報設定
PlayerConfig BulletPlayer::GetPlayerConfig()
{
	// 裏プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 2.0f;
	config.normalMoveSpeed = 4.5f;
	config.dashMoveSpeed = 2.0f;

	// 基礎ステータス
	config.life = 100.0f;
	config.maxLife = 100.0f;
	config.startPos = VGet(0.0f, 0.0f, 0.0f);

	// 表示設定
	config.drawSizeOffset = 16;
	config.drawOffsetX = 900;
	config.drawOffsetY = 0;

	// モデル名
	config.modelName = "BulletPlayer";

	return config;
}

// 弾プレイヤーのアニメーション設定
PlayerAnimation BulletPlayer::GetPlayerAnimation()
{
	// 裏プレイヤー用のアニメーション設定
	PlayerAnimation animation;

	animation.wait = "player_idle_01";
	animation.walk = "player_walk_01";
	animation.run = "player_jog_01";
	animation.jumpUp = "jump_up";
	animation.jumpDown = "jump_down";
	animation.crouchWait = "crouch_idle";
	animation.crouchWalk = "crouch";
	animation.hit = "player_damage_00";
	animation.dodge = "dodge";
	animation.death = "player_dead_00";

	return animation;
}

// 弾プレイヤーの表示設定
RenderConfig BulletPlayer::GetRenderConfig()
{
	// 裏プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Bullet Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

// 弾プレイヤーの攻撃定数設定
AttackConstants BulletPlayer::GetAttackConstants()
{
	// 弾プレイヤーは攻撃を行わない

	AttackConstants constants;

	constants.ATTACK_OFFSET_SCALE = 0.0f;
	constants.SURFACE_MAX_COMBO_COUNT = 0;
	constants.INTERIOR_MAX_COMBO_COUNT = 0;

	return constants;
}

// 弾プレイヤーの攻撃設定
void BulletPlayer::GetAttackConfigs(AttackConfig configs[])
{
	// 弾プレイヤーは攻撃を行わない
}

// 入力確認
void BulletPlayer::ShootByInput()
{
	if(_trg & PAD_INPUT_7)
	{
		ShootBullet();
	}
}

// 弾の発射
void BulletPlayer::ShootBullet()
{
	// 弾管理クラスの有効確認
	auto bulletManager = _bulletManager.lock();
	if(bulletManager)
	{
		// 弾情報設定
		bulletManager->Shoot
		(
			VAdd(_vPos, bulletConfig::START_POS_OFFSET),
			_vDir, 
			bulletConfig::RADIUS, 
			bulletConfig::SPEED, 
			bulletConfig::LIFE_TIME, 
			_eCharaType
		);
	}
}