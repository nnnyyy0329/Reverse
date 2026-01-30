#include "BulletPlayer.h"

BulletPlayer::BulletPlayer()
{
	_eCharaType = CHARA_TYPE::BULLET;
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
	CharaBase::ApplyDamage(fDamage, eType);
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
	config.startPos = VGet(-230.0f, 0.0f, 0.0f);

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

	animation.wait			= "";
	animation.walk			= "";
	animation.run			= "";
	animation.jumpUp		= "jump_up";
	animation.jumpDown		= "jump_down";
	animation.crouchWait	= "crouch_idle";
	animation.crouchWalk	= "crouch";
	animation.hit			= "";
	animation.dodge			= "dodge";
	animation.death			= "";

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