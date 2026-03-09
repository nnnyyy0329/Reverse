#include "BulletPlayer.h"
#include "CameraManager.h"

// 弾発射に関する定数
namespace BulletConstants
{
	constexpr float CONSUME_NORMAL_BULLET_ENERGY = 7.5f;	// 通常弾のエネルギー消費量
	constexpr float CONSUME_PIERCING_BULLET_ENERGY = 10.0f;	// 貫通弾のエネルギー消費量
}

// 弾発射設定
namespace bulletConfig
{
	const VECTOR RIGHT_ARM_SHOT_OFFSET = VGet(25, 80, 0);
	const VECTOR LEFT_ARM_SHOT_OFFSET = VGet(-15, 80, 0);
	constexpr float RADIUS = 20.0f;
	constexpr float DAMAGE = 50.0f;
	constexpr float SPEED = 15.0f;
	constexpr float LIFE_TIME = 120.0f;
}

BulletPlayer::BulletPlayer()
{
	_eCharaType = CHARA_TYPE::BULLET_PLAYER;

	_shootIntervalTimer = 0.0f;		// 発射間隔タイマー
	_bIsShootFromRightArm = false;	// 右腕から発射したかどうか
	_bIsReadyCompleted = false;		// 構えアニメーション完了フラグ
	_bWasShootKeyPressed = false;	// 前フレームで発射キーが押されていたか
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

	// 弾発射時間のデバッグ表示
	DrawShootIntervalTime();
}

// 被ダメージ処理
void BulletPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo)
{
	// 基底クラスの被ダメージ処理を呼び出す
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 弾による被ダメージ処理
void BulletPlayer::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)
{
	// 基底クラスの弾による被ダメージ処理を呼び出す
	PlayerBase::ApplyDamageByBullet(fDamage, chara);
}

// 弾プレイヤーの情報設定
PlayerConfig BulletPlayer::GetPlayerConfig()
{
	// 裏プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 2.0f;
	config.normalMoveSpeed = 4.5f;
	config.dashMoveSpeed = 3.5f;

	// 基礎ステータス
	config.life = 200.0f;
	config.maxLife = 200.0f;

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

	animation.movement.wait			= "player_idle_02";
	animation.movement.walk			= "player_walk_02";
	animation.movement.run			= "player_jog_02";
	animation.movement.jumpUp		= "";
	animation.movement.jumpDown		= "";
	animation.movement.crouchWait	= "";
	animation.movement.crouchWalk	= "";
	animation.shoot.shootReady		= "Schange_set_00";
	animation.shoot.rightArmShoot	= "Schange_attack_01";
	animation.shoot.leftArmShoot	= "Schange_attack_00";
	animation.shoot.shootMove		= "shoot_move";
	animation.combat.transform		= "";
	animation.combat.transCancel	= "player_cancell_01";
	animation.combat.guard			= "";
	animation.combat.hit			= "player_damage_02";
	animation.combat.dodge			= "player_dodge_02";
	animation.combat.death			= "player_dead_02";

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

// 回避設定データ構造体
DodgeConfig BulletPlayer::GetDodgeConfig()
{
	// 弾プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType			= DODGE_CHARA::BULLET_PLAYER;
	config.invincibleDuration	= 20.0f;	// 無敵時間
	config.startTime			= 3.0f;		// 開始時間
	config.activeTime			= 15.0f;	// アクティブ時間
	config.recoveryTime			= 8.0f;		// 硬直時間
	config.dodgeMoveSpeed		= 19.0f;	// 移動速度

	return config;
}

// 弾発射設定
BulletConfig BulletPlayer::GetBulletConfig()
{
	// オフセット値をワールド座標に変換
	VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(GetShootOffset(), GetShootDirection());
	

	// 弾プレイヤー用の弾発射設定
	BulletConfig config;

	config.bulletType	= BULLET_TYPE::NORMAL;			// 弾のタイプ
	config.shooterType	= CHARA_TYPE::BULLET_PLAYER;	// キャラタイプ
	config.startPos		= VAdd(_vPos, worldOffset);		// 発射開始位置
	config.dir			= GetShootDirection();			// エイムカメラの向いてる方向
	config.radius		= bulletConfig::RADIUS;			// 弾の半径
	config.damage		= bulletConfig::DAMAGE;			// 弾のダメージ
	config.speed		= bulletConfig::SPEED;			// 弾のスピード
	config.lifeTime		= bulletConfig::LIFE_TIME;		// 弾の自然消滅時間

	return config;
}

// 弾演出設定
BulletEffectConfig BulletPlayer::GetBulletEffectConfig()
{
	// 弾プレイヤー用の弾演出設定
	BulletEffectConfig config;

	config.effectName = "";							// エフェクトの名前
	config.effectOffset = VGet(0.0f, 0.0f, 0.0f);	// エフェクトの発生位置オフセット
	config.soundName = "";							// サウンドの名前

	return config;
}

// 弾発射カウント表示関数
void BulletPlayer::DrawShootIntervalTime()
{
	if(IsShootIntervalNegative()){ return; }

	// 弾発射カウントを表示
	DrawFormatString(10, 510, GetColor(255, 255, 255), "弾発射カウント: %3.2f", _shootIntervalTimer);
}

// 発射間隔更新
void BulletPlayer::ProcessShoot()
{
	// エイムモード開始
	_cameraManager->StartAimMode();

	// 弾発射の入力処理
	ShootInput();

	// 前フレームのキー状態を保存
	_bWasShootKeyPressed = IsShootInput();
}

// 弾発射の入力処理
void BulletPlayer::ShootInput()
{
	// キーが押された
	if(IsShootInput())
	{
		// 発射キーが新しく押され、発射カウントが残っていないなら
		if(!_bWasShootKeyPressed && IsShootIntervalNegative())
		{
			// 構えに移行
			_playerState.shootState = PLAYER_SHOOT_STATE::SHOOT_READY;	// 発射構え状態に移行
			_bIsReadyCompleted = false;									// 構えアニメーション未完了

			// アニメーション切り替え
			ProcessPlayAnimation();
		}
		// 構え状態で構えアニメーションが完了しているなら
		else if(_playerState.shootState == PLAYER_SHOOT_STATE::SHOOT_READY && !_bIsReadyCompleted)
		{
			// アニメーションが完了したかチェック
			if(!IsAnimationFinishedConst()){ return; }

			_bIsReadyCompleted = true; // 構えアニメーション完了フラグを立てる
		}
		// 発射構え状態が完了している、または既に構え完了状態で継続押下していて、発射カウントが0なら、
		else if((_bIsReadyCompleted || _bWasShootKeyPressed) && IsShootIntervalNegative())
		{
			// 発射状態の設定
			if(!_bIsShootFromRightArm)
			{
				_playerState.shootState = PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT; // 左腕発射
			}
			else
			{
				_playerState.shootState = PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT; // 右腕発射
			}

			// アニメーション切り替え
			ProcessPlayAnimation();

			// 弾の発射
			ShootBullet();

			// 発射間隔の設定
			AnimManager* animManager = GetAnimManager();
			_shootIntervalTimer = animManager->GetCurrentAnimTotalTime();
		}
	}
	// キーが離された
	else
	{
		// 発射構えアニメーションが完了していない状態でキーが離されたら
		if(!_bIsReadyCompleted && _bIsReadyCompleted)
		{
			// 発射状態をリセット
			_playerState.shootState = PLAYER_SHOOT_STATE::NONE;	// 発射状態リセット
		}

		// 発射カウントが0になったら
		if(IsShootIntervalNegative())
		{
			// キーが離されたら発射状態をリセット
			if(_playerState.shootState != PLAYER_SHOOT_STATE::NONE)
			{
				_playerState.shootState = PLAYER_SHOOT_STATE::NONE;	// 発射状態リセット
				_bIsReadyCompleted = false;							// 構えアニメーション完了フラグをリセット	
			}
		}

		_bIsShootFromRightArm = false; // 左腕から発射するようにリセット
	}

	// 弾発射カウントを減らす
	if(_shootIntervalTimer > 0.0f)
	{
		_shootIntervalTimer--; // カウントを減らす

		// 発射カウントが0になったら
		if(IsShootIntervalNegative())
		{
			// 発射アニメーション完了後、キーが押されていたら
			if(IsShootInput())
			{
				// 発射状態の設定
				_playerState.shootState = PLAYER_SHOOT_STATE::NONE;	// 発射状態リセット
			}
			// 発射状態のリセット
			else
			{
				// 古いステートを保存
				PlayerState oldState = _playerState;

				// 状態リセット
				_playerState.StateReset();

				_bIsReadyCompleted = false; // 構えアニメーション完了フラグをリセット

				// アニメーション更新
				_oldPlayerState = oldState;		// 変更前の状態を設定
				ProcessPlayAnimation();			// アニメーション更新
				_oldPlayerState = _playerState; // 更新後の状態に同期
			}
		}
	}
}

// 弾の発射
void BulletPlayer::ShootBullet()
{
	auto bullet = BulletManager::GetInstance();

	// 弾の発射
	bullet->Shoot(GetBulletConfig(), GetBulletEffectConfig(), BULLET_OWNER_TYPE::BULLET_PLAYER);

	// 発射時にエネルギー消費
	EnergyManager* energyManager = EnergyManager::GetInstance();
	if(energyManager)
	{
		energyManager->ConsumeEnergy(BulletConstants::CONSUME_NORMAL_BULLET_ENERGY);
	}

	_bIsShootFromRightArm = !_bIsShootFromRightArm; // 右腕と左腕の切り替え
}

// エイムモードの処理
void BulletPlayer::ProcessAimMode(bool aimKey)
{
	// エイムキーが押されているなら
	if(aimKey)
	{
		// エイムキーが押されたら
		if(_cameraManager && !_cameraManager->IsAimMode())
		{
			// エイムモード開始
			_cameraManager->StartAimMode();	// エイムモード開始
		}
	}
	// エイムキーが押されていないなら
	else
	{
		// エイムキーが離されたら
		if(_cameraManager && _cameraManager->IsAimMode())
		{
			// エイムモード終了
			_cameraManager->EndAimMode();	// エイムモード終了
		}
	}
}

// 発射位置オフセットの取得
VECTOR BulletPlayer::GetShootOffset()const
{
	// 発射位置オフセット設定
	VECTOR startPosOffset;

	// 右腕か左腕かでオフセットを変更
	if(_bIsShootFromRightArm){ startPosOffset = bulletConfig::RIGHT_ARM_SHOT_OFFSET; }	// 右腕
	else{ startPosOffset = bulletConfig::LEFT_ARM_SHOT_OFFSET; }						// 左腕

	// 発射位置オフセットを返す
	return startPosOffset;
}

// エイム中かどうかで発射方向を決める
VECTOR BulletPlayer::GetShootDirection()const
{
	// エイム中かどうかで発射方向を決定
	VECTOR shootDirection = _vDir;

	if(_cameraManager && _cameraManager->IsAimMode())
	{
		// エイム中はカメラの向いている方向に発射
		shootDirection = _cameraManager->GetAimDirection();
	}

	// 発射方向を返す
	return shootDirection;
}

// 弾発射入力チェック
bool BulletPlayer::IsShootInput()const
{
	auto inputManager = InputManager::GetInstance();

	// 攻撃ボタンが押されたか
	return (inputManager->IsHold(INPUT_ACTION::ATTACK));
}

// カウントが0より少ないかどうが
bool BulletPlayer::IsShootIntervalNegative()const
{
	// カウントが0より少ないかチェック
	if(_shootIntervalTimer <= 0.0f)
	{
		// カウントが0以下なら発射可能
		return true;
	}

	return false;
}