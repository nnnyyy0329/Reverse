// 担当 : 成田

#include "PlayerBase.h"
#include "CameraManager.h" 

// 共通関数呼び出し
void PlayerBase::CallProcess()
{
	// プレイヤー移動処理
	ProcessMovePlayer();

	// 被弾処理
	ProcessHit();

	// ステータスに応じたアニメーション処理
	ProcessStatusAnimation();

	// コリジョン位置の更新処理
	ProcessCollisionPos();

	// デバッグ用の処理
	ProcessDebug();
}

// プレイヤー移動処理
void PlayerBase::ProcessMovePlayer()
{
	_vOldPos = _vPos;	// 前フレームの位置を保存
	_vMove = { 0,0,0 };	// 移動方向を決める

	if(IsAttacking()){ return; }				// 攻撃中は移動入力を受け付けない
	//if(IsShooting()){ return; }					// 発射中は移動入力を受け付けない
	if(IsDodging()){ return; }					// 回避中は移動入力を受け付けない
	if(IsHitStop()){ return; }					// 被弾中は移動入力を受け付けない
	if(IsDeath()){ return; }					// 死亡中は移動入力を受け付けない

	bool isAiming = _cameraManager->IsAimMode();	// エイムモード中かどうか
	bool isShooting = IsShooting();					// 発射中かどうか
	if(!isAiming && isShooting){ return; }			// 発射中でエイムモードでない場合は移動入力を受け付けない

	// 移動処理
	{
		//if(_key & PAD_INPUT_DOWN) { _vMove.z = 1; }
		//if(_key & PAD_INPUT_UP) { _vMove.z = -1; }
		//if(_key & PAD_INPUT_LEFT) { _vMove.x = 1; }
		//if(_key & PAD_INPUT_RIGHT) { _vMove.x = -1; }
		
		// しゃがみ中かどうかで移動速度を変える
		if(_bIsCrouching)
		{
			_fMoveSpeed = _playerConfig.crouchMoveSpeed;
		}
		else
		{
			_fMoveSpeed = _playerConfig.normalMoveSpeed;
		}

		// 入力に応じた移動処理
		ProcessInputMove();

		// 位置更新
		_vPos = VAdd(_vPos, VScale(_vMove, _fMoveSpeed));
	}

	// モデル位置更新
	AnimManager* animManager = GetAnimManager();
	if(animManager != nullptr)
	{
		MV1SetPosition(animManager->GetModelHandle(), _vPos);
	}
}

// 入力に応じた移動処理
void PlayerBase::ProcessInputMove()
{
	// ダッシュ入力があれば移動速度を上げる
	if(_trg & PAD_INPUT_9)
	{
		_bIsDashInput = !_bIsDashInput;	// ダッシュ入力フラグをトグルする
	}

	// アナログ入力による移動
	if(abs(_lx) > _analogMin || abs(_ly) > _analogMin)
	{
		float currentCameraAngle;	// 現在のカメラの水平角度

		// カメラマネージャーがあれば
		if(_cameraManager)
		{
			// 現在のカメラの水平角度を取得して移動方向を変換する
			currentCameraAngle = _cameraManager->GetCurrentCameraAngleH();
		}
		// なければプレイヤーのカメラ角度を使用する
		else
		{
			currentCameraAngle = _cameraAngle;
		}

		VECTOR cameraForward;	// カメラの向いている方向のベクトル
		VECTOR cameraRight;		// カメラの右方向のベクトル

		// カメラタイプによってベクトルを変える
		switch(_cameraManager->GetCameraType())
		{
			case CAMERA_TYPE::GAME_CAMERA: // ゲームカメラ
			{
				// カメラの向いている方向のベクトル
				cameraForward = VGet(cos(currentCameraAngle), 0.0f, sin(currentCameraAngle));
				cameraRight = VGet(cos(currentCameraAngle + DX_PI_F / 2.0f), 0.0f, sin(currentCameraAngle + DX_PI_F / 2.0f));

				break;
			}

			case CAMERA_TYPE::AIM_CAMERA: // エイムカメラ
			{
				// カメラの向いている方向のベクトル
				cameraForward = VGet(-sin(currentCameraAngle), 0.0f, -cos(currentCameraAngle));
				cameraRight = VGet(cos(currentCameraAngle), 0.0f, -sin(currentCameraAngle));

				break;
			}
		}

		// 移動量を計算
		_vMove = VAdd
		(
			VScale(cameraForward, _ly),	// 前後移動
			VScale(cameraRight, _lx)	// 左右移動
		);

		// 正規化
		float len = VSize(_vMove);
		if(len > 0.0f)
		{
			_vMove = VScale(_vMove, 1.0f / len);
		}

		// ダッシュ入力があれば移動速度を上げる
		if(_bIsDashInput)
		{
			_fMoveSpeed += _playerConfig.dashMoveSpeed;
		}
	}
	// 移動入力がないなら
	else
	{
		_bIsDashInput = false;	// ダッシュ入力フラグを下げる
	}
}

// 状態変化アニメーション処理
void PlayerBase::ProcessStatusAnimation()
{
	// 攻撃ステート、先頭ステート時はここで再生処理
	if((_playerState.IsStateAttacking() ||
		_playerState.IsStateCombat()))
	{
		// アニメーション再生処理のみ
		ProcessPlayAnimation();

		// 処理後の攻撃中のステータスを保存
		_oldPlayerState = _playerState;

		return;
	}

	_oldPlayerState = _playerState;	// 処理前のステータスを保存しておく

	// 空中ならジャンプステータス
	if(!_bIsStanding)
	{
		if(_fVelY > 0.0f) // 上昇中
		{
			_playerState.movementState = PLAYER_MOVEMENT_STATE::JUMP_UP;
		}
		else // 下降中
		{
			_playerState.movementState = PLAYER_MOVEMENT_STATE::JUMP_DOWN;
		}
	}
	// 地上にいる場合
	else
	{
		bool isWalking = VSize(VGet(_vMove.x, 0.0f, _vMove.z)) > 0.0f;	// 移動入力があるかどうかをチェック
		bool isAiming = _cameraManager && _cameraManager->IsAimMode();	// エイム中かどうかをチェック

		// 走っているなら
		if(isWalking && _bIsDashInput)
		{
			// エイム中でなければ移動方向を向く
			if(!isAiming)
			{
				_vDir = _vMove;	// 移動方向を向く
			}

			_playerState.movementState = PLAYER_MOVEMENT_STATE::RUN;	// 走行
		}
		else if(isWalking) // 歩いているなら
		{
			// エイム中でなければ移動方向を向く
			if(!isAiming)
			{
				_vDir = _vMove;	// 移動方向を向く
			}

			_playerState.movementState = PLAYER_MOVEMENT_STATE::WALK;	// 歩行
		}
		else // 止まっているなら
		{
			_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;	// 待機
		}
	}

	// アニメーション再生処理
	ProcessPlayAnimation();
}

// アニメーション処理
void PlayerBase::ProcessPlayAnimation()
{
	// ステータスが変化していなければ処理しない
	if(!HasStateChanged()){ return; }

	// アニメーションマネージャーの取得
	AnimManager* animManager = GetAnimManager();
	if(animManager == nullptr){ return; }

	const char* animName = GetCurrentAnimationName();	// 現在のステータスに応じたアニメーション名を取得
	float blendTime = 1.0f;								// ブレンド時間
	int loopCnt = GetLoopCount();						// ループカウント
	if(animName != nullptr)
	{
		// アニメーションの切り替え
		animManager->ChangeAnimationByName(animName, blendTime, loopCnt);
	}
}

// コリジョン位置更新処理
void PlayerBase::ProcessCollisionPos()
{
	AnimManager* animManager = GetAnimManager();
	VECTOR modelPos = _vPos;

	if(animManager != nullptr)
	{
		modelPos = MV1GetPosition(animManager->GetModelHandle());
	}

	// しゃがみ状態で当たり判定の高さを変更
	if(!_bIsCrouching) // 立っている場合
	{
		_vCollisionBottom = VAdd(_vPos, VGet(0.0f, _fCollisionR, 0.0f));// 半径分ずらして中心位置に
		_vCollisionTop = VAdd(_vPos, VGet(0.0f, _fCollisionHeight - _fCollisionR, 0.0f));// 高さ分ずらす
	}
	else // しゃがみ中なら
	{
		_vCollisionBottom = VAdd(_vPos, VGet(0.0f, _fCollisionR, 0.0f));// 半径分ずらして中心位置に
		_vCollisionTop = VAdd(_vPos, VGet(0.0f, _fCollisionHeight - _fCollisionR, 0.0f));// 高さ分ずらす
	}
}

// 被弾処理
void PlayerBase::ProcessHit()
{
	if(_playerState.combatState != PLAYER_COMBAT_STATE::HIT){ return; }	// 被弾状態でない場合は処理しない

	_fHitTime -= 1.0f;	// 被弾時間を減らす

	// 被弾時間が終了したら通常状態に戻る
	if(_fHitTime <= 0.0f)
	{
		PlayerState oldStatus = _playerState; // 古いステータスを保存

		_playerState.combatState = PLAYER_COMBAT_STATE::NONE;		// 戦闘状態を通常に変更
		_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;	// ステータスを待機に変更

		_fHitSpeed = 0.0f;											// 吹き飛び速度を0にする
		_vHitDir = VGet(0, 0, 0);									// 吹き飛び方向をリセット

		// ステータス変更後、アニメーション切り替え
		_oldPlayerState = oldStatus;	// 古いステータスを攻撃状態に設定
		ProcessPlayAnimation();			// アニメーション切り替え実行
		_oldPlayerState = _playerState;	// 切り替え後に更新

		return;
	}

	// 吹き飛び処理
	if(_fHitSpeed > 0.0f)
	{
		// のけぞり方向に移動
		_vPos = VAdd(_vPos, VScale(_vHitDir, _fHitSpeed));

		// 吹き飛び速度減衰
		_fHitSpeed *= _fHitSpeedDecay;

		// 速度が一定以下になったら0にする
		if(_fHitSpeed < 0.1f)
		{
			_fHitSpeed = 0.0f;
		}
	}

	// モデル位置更新
	AnimManager* animManager = GetAnimManager();
	if(animManager != nullptr)
	{
		MV1SetPosition(animManager->GetModelHandle(), _vPos);
	}
}

// デバッグ処理
void PlayerBase::ProcessDebug()
{
	if(_trg & PAD_INPUT_4)
	{
		_fLife -= 5.0f;
	}

	if(_trg & PAD_INPUT_2)
	{
		EnergyManager::GetInstance()->ConsumeEnergy(10.0f);
	}

	if (_trg & PAD_INPUT_5)
	{
		EnergyManager::GetInstance()->AddEnergy(100.0f);
	}
}

// 被弾硬直中かチェック
bool PlayerBase::IsHitStop()
{
	// 被弾硬直中かチェック
	if(_oldPlayerState.combatState == PLAYER_COMBAT_STATE::HIT)
	{
		return true;
	}

	return false;
}

const char* PlayerBase::GetCurrentAnimationName() const
{
	/* 優先度: 戦闘状態 > 攻撃状態 > 発射状態 > 移動状態 */ 

	if(_playerState.combatState != PLAYER_COMBAT_STATE::NONE)
	{
		switch(_playerState.combatState)
		{
			case PLAYER_COMBAT_STATE::TRANSFORM:	return _playerAnim.combat.transform;
			case PLAYER_COMBAT_STATE::TRANS_CANCEL:	return _playerAnim.combat.transCancel;
			case PLAYER_COMBAT_STATE::GUARD:		return _playerAnim.combat.guard;
			case PLAYER_COMBAT_STATE::HIT:			return _playerAnim.combat.hit;
			case PLAYER_COMBAT_STATE::DODGE:		return _playerAnim.combat.dodge;
			case PLAYER_COMBAT_STATE::DEATH:		return _playerAnim.combat.death;
		}
	}

	if(_playerState.attackState != PLAYER_ATTACK_STATE::NONE)
	{
		switch(_playerState.attackState)
		{
			case PLAYER_ATTACK_STATE::FIRST_ATTACK:		return _playerAnim.attack.firstAttack;
			case PLAYER_ATTACK_STATE::SECOND_ATTACK:	return _playerAnim.attack.secondAttack;
			case PLAYER_ATTACK_STATE::THIRD_ATTACK:		return _playerAnim.attack.thirdAttack;
			case PLAYER_ATTACK_STATE::FOURTH_ATTACK:	return _playerAnim.attack.fourthAttack;
			case PLAYER_ATTACK_STATE::FIFTH_ATTACK:		return _playerAnim.attack.fifthAttack;
		}
	}

	if(_playerState.shootState != PLAYER_SHOOT_STATE::NONE)
	{
		switch(_playerState.shootState)
		{
			case PLAYER_SHOOT_STATE::SHOOT_READY:		return _playerAnim.shoot.shootReady;
			case PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT:	return _playerAnim.shoot.rightArmShoot;
			case PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT:	return _playerAnim.shoot.leftArmShoot;
			case PLAYER_SHOOT_STATE::SHOOT_MOVE:		return _playerAnim.shoot.shootMove;
		}
	}

	switch(_playerState.movementState)
	{
		case PLAYER_MOVEMENT_STATE::WAIT:			return _playerAnim.movement.wait;
		case PLAYER_MOVEMENT_STATE::WALK:			return _playerAnim.movement.walk;
		case PLAYER_MOVEMENT_STATE::RUN:			return _playerAnim.movement.run;
		case PLAYER_MOVEMENT_STATE::JUMP_UP:		return _playerAnim.movement.jumpUp;
		case PLAYER_MOVEMENT_STATE::JUMP_DOWN:		return _playerAnim.movement.jumpDown;
		case PLAYER_MOVEMENT_STATE::CROUCH_WAIT:	return _playerAnim.movement.crouchWait;
		case PLAYER_MOVEMENT_STATE::CROUCH_WALK:	return _playerAnim.movement.crouchWalk;
	}

	return nullptr;
}

// ループカウント取得関数
int PlayerBase::GetLoopCount() const
{
	// 1: 1回再生 、 0: ループ再生
	
	// 戦闘状態と
	// 攻撃状態と
	// 発射状態の場合1回再生
	if(_playerState.combatState	 != PLAYER_COMBAT_STATE::NONE ||
		_playerState.attackState != PLAYER_ATTACK_STATE::NONE ||
		_playerState.shootState	 != PLAYER_SHOOT_STATE::NONE)
	{
		return 1;
	}
	
	// そのほかはループ再生
	return 0;
}

// 状態変化チェック
bool PlayerBase::HasStateChanged()const
{
	// ステートが変化しているならtrue
	return (_oldPlayerState.movementState != _playerState.movementState ||	// 移動状態
			_oldPlayerState.attackState   != _playerState.attackState	||	// 攻撃状態
			_oldPlayerState.shootState    != _playerState.shootState	||	// 発射状態
			_oldPlayerState.combatState   != _playerState.combatState);		// 戦闘状態
}