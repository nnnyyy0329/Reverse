#include "BulletPlayer.h"
#include "CameraManager.h"

// 弾発射設定エイリアス
namespace BC =  BulletConstants;

// 通常弾設定エイリアス
namespace NBC = NormalBulletConfig;

// 貫通弾設定エイリアス
namespace PBC = PiercingBulletConfig;

BulletPlayer::BulletPlayer()
{
	_eCharaType = CHARA_TYPE::BULLET_PLAYER;

	_shootIntervalTimer = 0.0f;		// 発射間隔タイマー
	_bIsShootFromRightArm = false;	// 右腕から発射したかどうか
	_bIsReadyCompleted = false;		// 構えアニメーション完了フラグ
	_bWasShootKeyPressed = false;	// 前フレームで発射キーが押されていたか

	_currentBulletType = BULLET_TYPE::NORMAL;	// 初期の弾の種類は通常弾
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

	bool isAiming = (_cameraManager && _cameraManager->GetCameraType() == CAMERA_TYPE::AIM_CAMERA);

	if (isAiming)
	{
		// カメラマネージャーから現在のエイム方向を取得
		VECTOR aimDir = _cameraManager->GetCameraDir();

		// Y軸成分を無視して水平にする
		VECTOR aimDirHorizontal = VGet(aimDir.x, 0.0f, aimDir.z);

		// 正規化してプレイヤーの向きにセットする
		if (VSquareSize(aimDirHorizontal) > 0.0f)
		{
			aimDirHorizontal = VNorm(aimDirHorizontal);

			SetDir(aimDirHorizontal);
		}
	}
	else
	{
		// 通常移動時
		float moveSquare = VSquareSize(_vMove);
		if (moveSquare > 0.0f)
		{
			SetDir(VNorm(_vMove));
		}
	}


	return true;
}

bool BulletPlayer::Render()
{
	PlayerBase::Render();

	return true;
}

void BulletPlayer::DebugRender()
{
	PlayerBase::DebugRender();

	// 弾発射時間のデバッグ表示
	DrawShootIntervalTime();
}

void BulletPlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo)
{
	// 基底クラスの被ダメージ処理を呼び出す
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

void BulletPlayer::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara, BULLET_TYPE bulletType)
{
	// 基底クラスの弾による被ダメージ処理を呼び出す
	PlayerBase::ApplyDamageByBullet(fDamage, chara, bulletType);
}

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

RenderConfig BulletPlayer::GetRenderConfig()
{
	// 裏プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Bullet Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

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

BulletConfig BulletPlayer::GetBulletConfig()
{
	// オフセット値をワールド座標に変換
	VECTOR worldOffset = GeometryUtility::TransOffsetToWorld(GetShootOffset(), GetShootDirection());

	// 弾プレイヤー用の弾発射設定
	BulletConfig config;

	// 弾の種類による設定
	switch(_currentBulletType)
	{
		case BULLET_TYPE::NORMAL: // 通常弾
		{
			// 通常弾設定
			config.bulletType	= BULLET_TYPE::NORMAL;	// 弾の種類
			config.radius		= NBC::RADIUS;			// 弾の半径
			config.damage		= NBC::DAMAGE;			// ダメージ量
			config.speed		= NBC::SPEED;			// 移動速度

			break;
		}

		case BULLET_TYPE::PIERCING: // 貫通弾
		{
			// 貫通弾設定
			config.bulletType	= BULLET_TYPE::PIERCING;	// 弾の種類
			config.radius		= PBC::RADIUS;				// 弾の半径
			config.damage		= PBC::DAMAGE;				// ダメージ量
			config.speed		= PBC::SPEED;				// 移動速度

			break;
		}

		case BULLET_TYPE::NONE: // 弾なし
		default:
		{
			break;
		}
	}
	config.shooterType	= CHARA_TYPE::BULLET_PLAYER;	// キャラタイプ
	config.startPos		= VAdd(_vPos, worldOffset);		// 発射開始位置
	config.dir			= GetShootDirection();			// エイムカメラの向いてる方向
	config.lifeTime		= BC::LIFE_TIME;				// 弾の自然消滅時間

	return config;
}

BulletEffectConfig BulletPlayer::GetBulletEffectConfig()
{
	// 弾プレイヤー用の弾演出設定
	BulletEffectConfig config;

	config.effectName = "";							// エフェクトの名前
	config.effectOffset = VGet(0.0f, 0.0f, 0.0f);	// エフェクトの発生位置オフセット
	config.soundName = "";							// サウンドの名前

	return config;
}

void BulletPlayer::DrawShootIntervalTime()
{
	// 発射間隔がマイナスなら表示しない
	if(IsShootIntervalNegative()){ return; }

	// 弾発射カウントを表示
	DrawFormatString(10, 510, GetColor(255, 255, 255), "弾発射カウント: %3.2f", _shootIntervalTimer);
}

void BulletPlayer::ProcessShoot()
{
	if (_cameraManager)
	{
		// エイムカメラに切り替える
		_cameraManager->SetCameraType(CAMERA_TYPE::AIM_CAMERA);
	}

	// 弾発射の入力処理
	ShootInput();

	// 前フレームのキー状態を保存
	_bWasShootKeyPressed = IsShootNormalInput() || IsShootPiercingInput();
}

void BulletPlayer::ShootInput()
{
	// どちらかの発射キーが新しく押されたかどうか
	bool isInputPress = IsShootNormalInput() || IsShootPiercingInput();	// 発射キーが押されているか
	bool isInputNewPress = isInputPress && !_bWasShootKeyPressed;		// 発射キーが新しく押されたか

	// 待機状態：新しく入力がきたら構えへ
	if(_playerState.shootState == PLAYER_SHOOT_STATE::NONE	&&	// 現在発射構え状態でなく
		isInputNewPress										&&	// 発射キーが新しく押され
		IsShootIntervalNegative())								// 発射カウントが残っていないなら
	{
		// 発射構え状態に移行
		_playerState.shootState = PLAYER_SHOOT_STATE::SHOOT_READY;

		// 構えアニメーション未完了
		_bIsReadyCompleted = false;

		// アニメーション切り替え
		ProcessPlayAnimation();

		return;
	}

	// 構え状態：アニメーション完了を待つ
	if(_playerState.shootState == PLAYER_SHOOT_STATE::SHOOT_READY &&
		!_bIsReadyCompleted)
	{
		// アニメーションがまだ再生中
		if(!IsAnimationFinishedConst()){ return; }

		// アニメーション完了
		_bIsReadyCompleted = true;

		return;
	}

	// 構え完了状態：発射可能になったら発射へ
	if(_playerState.shootState == PLAYER_SHOOT_STATE::SHOOT_READY	&&	// 現在発射構え状態で
		_bIsReadyCompleted											&&	// 構えアニメーションが完了していて
		IsShootIntervalNegative())										// 発射カウントが残っていないなら
	{
		// 発射状態への移行処理
		TransToShootState();

		return;
	}

	// 発射中(タイマー進行中)：カウント処理
	if(_shootIntervalTimer > 0.0f)
	{
		_shootIntervalTimer--;

		// タイマー終了時
		if(IsShootIntervalNegative())
		{
			// 発射状態の更新処理
			UpdateByShootIntervalEnd(isInputPress);
		}

		return;
	}

	// 入力が離されたら初期化
	if(!isInputPress && _playerState.shootState != PLAYER_SHOOT_STATE::NONE)
	{
		// 発射状態をリセット
		ResetShootState();

		return;
	}
}

void BulletPlayer::TransToShootState()
{
	// 発射状態の設定
	if(!_bIsShootFromRightArm)
	{
		// 左腕発射
		_playerState.shootState = PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT; 
	}
	else
	{
		// 右腕発射
		_playerState.shootState = PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT; 
	}

	// アニメーション切り替え
	ProcessPlayAnimation();

	// 弾の発射
	ShootBullet();

	// 発射間隔の設定
	AnimManager* animManager = GetAnimManager();
	_shootIntervalTimer = animManager->GetCurrentAnimTotalTime();
}

void BulletPlayer::UpdateByShootIntervalEnd(bool isInputActive)
{
	// まだ入力が継続していれば、次の発射へ
	if(isInputActive && IsShootIntervalNegative())
	{
		// 発射状態への移行処理
		TransToShootState();

		return;
	}

	// 入力が途切れていれば完全にリセット
	if(!isInputActive)
	{
		// 発射状態をリセット
		ResetShootState();

		return;
	}
}

void BulletPlayer::ResetShootState()
{
	PlayerState oldState = _playerState;
	
	// 発射状態をリセット
	_playerState.StateReset();

	// 構えアニメーション完了フラグをリセット
	_bIsReadyCompleted = false;

	// 右腕から発射するようにリセット
	_bIsShootFromRightArm = false;

	// アニメーション更新
	_oldPlayerState = oldState;		// 変更前の状態を設定
	ProcessPlayAnimation();			// アニメーション更新
	_oldPlayerState = _playerState;	// 更新後の状態に同期
}

void BulletPlayer::ShootBullet()
{
	auto bullet = BulletManager::GetInstance();

	// 弾の発射
	bullet->Shoot(GetBulletConfig(), GetBulletEffectConfig(), BULLET_OWNER_TYPE::BULLET_PLAYER);

	// 発射時にエネルギー消費
	EnergyManager* energyManager = EnergyManager::GetInstance();
	if(energyManager)
	{
		// エネルギーを消費
		energyManager->ConsumeEnergy(BulletConstants::CONSUME_NORMAL_BULLET_ENERGY);
	}

	// 右腕と左腕の切り替え
	_bIsShootFromRightArm = !_bIsShootFromRightArm; 
}

VECTOR BulletPlayer::GetShootOffset()const
{
	// 発射位置オフセット設定
	VECTOR startPosOffset;

	// 右腕か左腕かでオフセットを変更
	if(_bIsShootFromRightArm){ startPosOffset = BC::RIGHT_ARM_SHOT_OFFSET; }	// 右腕
	else{ startPosOffset = BC::LEFT_ARM_SHOT_OFFSET; }							// 左腕

	// 発射位置オフセットを返す
	return startPosOffset;
}

VECTOR BulletPlayer::GetShootDirection()const
{
	// エイム中かどうかで発射方向を決定
	VECTOR shootDirection = _vDir;

	bool isAiming = (_cameraManager && _cameraManager->GetCameraType() == CAMERA_TYPE::AIM_CAMERA);

	if(isAiming)
	{
		// エイム中はカメラの向いている方向に発射
		shootDirection = _cameraManager->GetCameraDir();
	}

	// 発射方向を返す
	return shootDirection;
}

bool BulletPlayer::IsShootNormalInput()
{
	auto& inputManager = InputManager::GetInstance();

	// 通常弾発射ボタンが押されたら
	if(inputManager.IsHold(INPUT_ACTION::ATTACK))
	{
		// 弾のタイプを通常弾にセット
		SetBulletType(BULLET_TYPE::NORMAL);

		return true;
	}

	return false;
}

bool BulletPlayer::IsShootPiercingInput()
{
	auto& inputManager = InputManager::GetInstance();

	// 貫通弾発射ボタンが押されたら
	if(inputManager.IsHold(INPUT_ACTION::ABILITY))
	{
		// 弾のタイプを貫通弾にセット
		SetBulletType(BULLET_TYPE::PIERCING);

		return true;
	}

	return false;
}

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

void BulletPlayer::SetBulletType(BULLET_TYPE bulletType)
{
	_currentBulletType = bulletType;
}