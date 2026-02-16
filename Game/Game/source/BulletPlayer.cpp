#include "BulletPlayer.h"
#include "BulletManager.h"
#include "CameraManager.h"

// 弾発射設定
namespace bulletConfig
{
	const VECTOR RIGHT_ARM_SHOT_OFFSET = VGet(25, 80, 0);
	const VECTOR LEFT_ARM_SHOT_OFFSET = VGet(-15, 80, 0);
	constexpr float RADIUS = 10.0f;
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
void BulletPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 発射間隔更新
void BulletPlayer::ProcessShoot()
{
	bool putKey = (_key & PAD_INPUT_6) != 0;	// 発射キー
	bool aimKey = (_key & PAD_INPUT_5) != 0;	// エイムキー

	// エイムモードの制御
	ProcessAimMode(aimKey);

	// キーが押された
	if(putKey)
	{
		// 発射キーが新しく押され、発射カウントが残っていないなら
		if(!_bWasShootKeyPressed && _shootIntervalTimer <= 0.0f)
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
			AnimManager* animManager = GetAnimManager();
			if(animManager->IsAnimationFinished())
			{
				_bIsReadyCompleted = true; // 構えアニメーション完了フラグを立てる
			}
		}
		// 発射構え状態が完了している、または既に構え完了状態で継続押下していて、発射カウントが0なら、
		else if((_bIsReadyCompleted || _bWasShootKeyPressed) && _shootIntervalTimer <= 0.0f)
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
		// 発射カウントが0になったら
		if(_shootIntervalTimer <= 0.0f)
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
		if(_shootIntervalTimer <= 0.0f)
		{
			// 発射アニメーション完了後、キーが押されていたら
			if(putKey)
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
				_playerState.shootState = PLAYER_SHOOT_STATE::NONE;
				_playerState.movementState = PLAYER_MOVEMENT_STATE::NONE;

				_bIsReadyCompleted = false; // 構えアニメーション完了フラグをリセット

				// アニメーション更新
				_oldPlayerState = oldState;		// 変更前の状態を設定
				ProcessPlayAnimation();			// アニメーション更新
				_oldPlayerState = _playerState; // 更新後の状態に同期
			}
		}
	}

	// 前フレームのキー状態を保存
	_bWasShootKeyPressed = putKey;
}

// 弾の発射
void BulletPlayer::ShootBullet()
{
	// 弾管理クラスの有効確認
	auto bulletManager = _bulletManager.lock();
	if(bulletManager)
	{
		// 発射位置オフセット設定
		VECTOR startPosOffset;

		// 右腕か左腕かでオフセットを変更
		if(_bIsShootFromRightArm){ startPosOffset = bulletConfig::RIGHT_ARM_SHOT_OFFSET; }	// 右腕
		else{ startPosOffset = bulletConfig::LEFT_ARM_SHOT_OFFSET; }						// 左腕

		// エイム中かどうかで発射方向を決定
		VECTOR shootDirection = _vDir;

		if(_cameraManager && _cameraManager->IsAimMode())
		{
			// エイム中はカメラの向いている方向に発射
			shootDirection = _cameraManager->GetAimDirection();
		}

		// 発射位置をワールド座標に変換
		VECTOR worldOffset = TransOffsetToWorld(startPosOffset, shootDirection);

		// 弾情報設定
		bulletManager->Shoot
		(
			VAdd(_vPos, worldOffset),
			shootDirection,
			bulletConfig::RADIUS,
			bulletConfig::SPEED,
			bulletConfig::LIFE_TIME,
			_eCharaType
		);
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
			_cameraManager->StartAimMode();
		}
	}
	// エイムキーが押されていないなら
	else
	{
		// エイムキーが離されたら
		if(_cameraManager && _cameraManager->IsAimMode())
		{
			// エイムモード終了
			_cameraManager->EndAimMode();
		}
	}
}

// オフセット位置をワールド座標に変換
VECTOR BulletPlayer::TransOffsetToWorld(const VECTOR& offset, const VECTOR& playerDir)
{
	// プレイヤーの向きベクトルの正規化
	VECTOR dirNorm = VNorm(playerDir);

	// 上ベクトル設定
	VECTOR upVec = VGet(0.0f, 1.0f, 0.0f);

	// 外積で右ベクトルを計算
	VECTOR rightVec = VCross(upVec, dirNorm);	

	// 右ベクトルの正規化
	rightVec = VNorm(rightVec);

	// ワールド座標に変換
	VECTOR worldPos = VAdd
	(
		// 右ベクトルと上ベクトルの合成位置
		VAdd
		(
			VScale(rightVec, offset.x),
			VScale(upVec, offset.y)
		),

		// 前方向ベクトルのスケーリング位置
		VScale(dirNorm, offset.z)
	);

	return worldPos;
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
	animation.combat.dodge			= "dodge";
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
	config.invincibleDuration = 20.0f;  // 無敵時間
	config.startTime = 3.0f;            // 開始時間
	config.activeTime = 15.0f;          // アクティブ時間
	config.recoveryTime = 8.0f;         // 硬直時間
	config.dodgeMoveSpeed = 19.0f;      // 移動速度

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

	DrawFormatString(10, 510, GetColor(255, 255, 255), "弾発射カウント: %3.2f", _shootIntervalTimer);
}