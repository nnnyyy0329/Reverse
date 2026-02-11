#include "BulletPlayer.h"
#include "BulletManager.h"

namespace bulletConfig
{
	const VECTOR RIGHT_ARM_SHOT_OFFSET = VGet(15, 70, 0);
	const VECTOR LEFT_ARM_SHOT_OFFSET = VGet(-15, 70, 0);
	const float RADIUS = 10.0f;
	const float SPEED = 15.0f;
	const float LIFE_TIME = 120.0f;
}

BulletPlayer::BulletPlayer()
{
	_eCharaType = CHARA_TYPE::BULLET_PLAYER;

	_shootIntervalTimer = 0.0f;
	_bIsShootFromRightArm = false;
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

	// 弾発射時間のデバッグ表示
	DrawShootIntervalTime();
}

// 被ダメージ処理
void BulletPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 入力確認
void BulletPlayer::ShootByInput()
{
	bool putKey = (_key & PAD_INPUT_6) != 0;
	
	// 発射間隔更新
	//ShootIntervalUpdate(putKey);
}

// 発射間隔更新
void BulletPlayer::ProcessShoot()
{
	bool putKey = (_key & PAD_INPUT_6) != 0;

	// キーが押された
	if(putKey)
	{
		// 発射カウントが残っているなら
		if(_shootIntervalTimer <= 0.0f)
		{
			// 発射状態を設定
			if(_bIsShootFromRightArm)
			{
				_playerState.shootState = PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT;
			}
			else
			{
				_playerState.shootState = PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT;
			}

			// 弾の発射
			ShootBullet();

			// 発射間隔の設定
			AnimManager* animManager = GetAnimManager();
			float animTotalTime = animManager->GetCurrentAnimTotalTime();
			_shootIntervalTimer = animTotalTime;
		}
	}
	// キーが離された
	else
	{
		// 発射カウントが0になったら
		if(_shootIntervalTimer <= 0.0f)
		{
			// キーが離されたら発射状態をリセット
			if(_playerState.shootState != PLAYER_SHOOT_STATE::NONE)
			{
				// 状態リセット
				_playerState.shootState = PLAYER_SHOOT_STATE::NONE;
				_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;
			}
		}
	}

	// 弾発射カウントを減らす
	if(_shootIntervalTimer > 0.0f)
	{
		_shootIntervalTimer--; // カウントを減らす
		if(_shootIntervalTimer <= 0.0f)
		{
			// 状態リセット
			_playerState.shootState = PLAYER_SHOOT_STATE::NONE;
			_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;
		}
	}
}

// 弾の発射
void BulletPlayer::ShootBullet()
{
	_bIsShootFromRightArm = !_bIsShootFromRightArm; // 右腕と左腕の切り替え

	// 弾管理クラスの有効確認
	auto bulletManager = _bulletManager.lock();
	if(bulletManager)
	{
		// 発射位置オフセット設定
		VECTOR startPosOffset;

		// 右腕か左腕かでオフセットを変更
		if(_bIsShootFromRightArm){ startPosOffset = bulletConfig::RIGHT_ARM_SHOT_OFFSET; }	// 右腕
		else{ startPosOffset = bulletConfig::LEFT_ARM_SHOT_OFFSET; }						// 左腕

		// 弾情報設定
		bulletManager->Shoot
		(
			VAdd(_vPos, startPosOffset),
			_vDir,
			bulletConfig::RADIUS,
			bulletConfig::SPEED,
			bulletConfig::LIFE_TIME,
			_eCharaType
		);
	}
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
PlayerAnimations BulletPlayer::GetPlayerAnimation()
{
	// 裏プレイヤー用のアニメーション設定
	PlayerAnimations animation;

	animation.movement.wait			= "player_idle_00";
	animation.movement.walk			= "player_walk_00";
	animation.movement.run			= "player_jog_00";
	animation.movement.jumpUp		= "";
	animation.movement.jumpDown		= "";
	animation.movement.crouchWait	= "";
	animation.movement.crouchWalk	= "";
	animation.shoot.rightArmShoot	= "shoot_right";
	animation.shoot.leftArmShoot	= "shoot_left";
	animation.shoot.shootMove		= "shoot_move";
	animation.combat.guard			= "";
	animation.combat.hit			= "player_damage_00";
	animation.combat.dodge			= "dodge";
	animation.combat.death			= "player_dead_00";

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

	constants.attackOffsetScale = 0.0f;
	constants.surfaceMaxComboCount = 0;
	constants.interiorMaxComboCount = 0;

	return constants;
}

// 弾プレイヤーの攻撃設定
void BulletPlayer::GetAttackConfigs(AttackConfig configs[])
{
	// 弾プレイヤーは攻撃を行わない
}

// 回避設定データ構造体
DodgeConfig BulletPlayer::GetDodgeConfig()
{
	// 弾プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::BULLET_PLAYER;
	config.invincibleDuration = 20.0f;  // 無敵時間（短め）
	config.startTime = 3.0f;            // 開始時間（早め）
	config.activeTime = 15.0f;          // アクティブ時間（短め）
	config.recoveryTime = 8.0f;         // 硬直時間（短め）
	config.dodgeMoveSpeed = 12.0f;      // 移動速度（高速）

	return config;
}

// シールド設定を取得
ShieldConfig BulletPlayer::GetShieldConfig()
{
	// デフォルトの設定を返す
	ShieldConfig config = {};

	return config;
}

// 弾発射カウント表示関数
void BulletPlayer::DrawShootIntervalTime()
{
	if(_shootIntervalTimer <= 0.0f){ return; }

	DrawFormatString(1100, 310, GetColor(255, 255, 255), "%3.2f", _shootIntervalTimer);
}