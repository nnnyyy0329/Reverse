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

	if(IsAttacking()){ return; }	// 攻撃中は移動入力を受け付けない
	if(IsDodging()){ return; }		// 回避中は移動入力を受け付けない
	if(IsHitStop()){ return; }		// 被弾中は移動入力を受け付けない
	if(IsDeath()){ return; }		// 死亡中は移動入力を受け付けない
	if(_playerState.IsStateAbsorbing()){ return; }	// 吸収攻撃中は移動入力を受け付けない

	bool isAiming = _cameraManager->IsAimMode();	// エイムモード中かどうか
	bool isShooting = IsShooting();					// 発射中かどうか
	if(!isAiming && isShooting){ return; }			// 発射中でエイムモードでない場合は移動入力を受け付けない

	// 移動処理
	{
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
	auto im = InputManager::GetInstance();

	// ダッシュ入力があれば移動速度を上げる
	if(im->IsTrigger(INPUT_ACTION::DASH))
	{
		_bIsDashInput = !_bIsDashInput;	// ダッシュ入力フラグをトグルする
	}

	const AnalogState& analog = im->GetAnalog();
	float analogMin = im->GetAnalogMin();

	// アナログ入力による移動
	if(abs(analog.lx) > analogMin || abs(analog.ly) > analogMin)
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
			VScale(cameraForward, analog.ly),	// 前後移動
			VScale(cameraRight, analog.lx)	// 左右移動
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
	float blendTime = 5.0f;								// ブレンド時間
	int loopCnt = GetLoopCount();						// ループカウント
	if(animName != nullptr)
	{
		// アニメーションの切り替え
		animManager->ChangeAnimationByName(animName, blendTime, loopCnt);
	}
}

// 通常モーションに戻す処理
void PlayerBase::ProcessReturnNormalMotion()
{
	// 攻撃状態と戦闘状態を通常に戻す
	_playerState.StateReset();

	_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;	// 移動状態は待機にする

	// アニメーション再生処理
	ProcessPlayAnimation();
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

		// ステータスを通常にリセット
		_playerState.StateReset();

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
	auto im = InputManager::GetInstance();

	if (im->IsTrigger(INPUT_ACTION::DEBUG3))
	{
		_fLife -= 5.0f;
	}

	if (im->IsTrigger(INPUT_ACTION::DEBUG1))
	{
		EnergyManager::GetInstance()->ConsumeEnergy(50.0f);
	}

	if (im->IsTrigger(INPUT_ACTION::DEBUG2))
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
			case PLAYER_COMBAT_STATE::TRANSFORM:	return _playerAnim.combat.transform;	// 変身
			case PLAYER_COMBAT_STATE::TRANS_CANCEL:	return _playerAnim.combat.transCancel;	// 変身解除
			case PLAYER_COMBAT_STATE::GUARD:		return _playerAnim.combat.guard;		// ガード
			case PLAYER_COMBAT_STATE::HIT:			return _playerAnim.combat.hit;			// 被弾	
			case PLAYER_COMBAT_STATE::DODGE:		return _playerAnim.combat.dodge;		// 回避
			case PLAYER_COMBAT_STATE::DEATH:		return _playerAnim.combat.death;		// 死亡
		}
	}

	if(_playerState.attackState != PLAYER_ATTACK_STATE::NONE)
	{
		switch(_playerState.attackState)
		{
			case PLAYER_ATTACK_STATE::FIRST_ATTACK:		return _playerAnim.attack.firstAttack;		// 1段目攻撃
			case PLAYER_ATTACK_STATE::SECOND_ATTACK:	return _playerAnim.attack.secondAttack;		// 2段目攻撃
			case PLAYER_ATTACK_STATE::THIRD_ATTACK:		return _playerAnim.attack.thirdAttack;		// 3段目攻撃
			case PLAYER_ATTACK_STATE::FOURTH_ATTACK:	return _playerAnim.attack.fourthAttack;		// 4段目攻撃
			case PLAYER_ATTACK_STATE::FIFTH_ATTACK:		return _playerAnim.attack.fifthAttack;		// 5段目攻撃
			case PLAYER_ATTACK_STATE::AREA_ATTACK:		return _playerAnim.attack.areaAttack;		// 範囲攻撃
			case PLAYER_ATTACK_STATE::FIRST_SKILL:		return _playerAnim.attack.firstSkill;		// スキル1
			case PLAYER_ATTACK_STATE::SECOND_SKILL:		return _playerAnim.attack.secondSkill;		// スキル2
		}
	}

	if(_playerState.shootState != PLAYER_SHOOT_STATE::NONE)
	{
		switch(_playerState.shootState)
		{
			case PLAYER_SHOOT_STATE::SHOOT_READY:		return _playerAnim.shoot.shootReady;	// 発射構え
			case PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT:	return _playerAnim.shoot.rightArmShoot;	// 右腕発射
			case PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT:	return _playerAnim.shoot.leftArmShoot;	// 左腕発射
			case PLAYER_SHOOT_STATE::SHOOT_MOVE:		return _playerAnim.shoot.shootMove;		// 発射移動
		}
	}

	if(_playerState.absorbState != PLAYER_ABSORB_STATE::NONE)
	{
		switch(_playerState.absorbState)
		{
			case PLAYER_ABSORB_STATE::ABSORB_READY:		return _playerAnim.absorb.absorbReady;	// 吸収開始
			case PLAYER_ABSORB_STATE::ABSORB_ACTIVE:	return _playerAnim.absorb.absorbActive;	// 吸収中
			case PLAYER_ABSORB_STATE::ABSORB_END:		return _playerAnim.absorb.absorbEnd;	// 吸収終了
		}
	}

	switch(_playerState.movementState)
	{
		case PLAYER_MOVEMENT_STATE::WAIT:			return _playerAnim.movement.wait;		// 待機
		case PLAYER_MOVEMENT_STATE::WALK:			return _playerAnim.movement.walk;		// 歩行
		case PLAYER_MOVEMENT_STATE::RUN:			return _playerAnim.movement.run;		// 走行
		case PLAYER_MOVEMENT_STATE::JUMP_UP:		return _playerAnim.movement.jumpUp;		// ジャンプ（上昇）
		case PLAYER_MOVEMENT_STATE::JUMP_DOWN:		return _playerAnim.movement.jumpDown;	// ジャンプ（下降）
		case PLAYER_MOVEMENT_STATE::CROUCH_WAIT:	return _playerAnim.movement.crouchWait;	// しゃがみ待機
		case PLAYER_MOVEMENT_STATE::CROUCH_WALK:	return _playerAnim.movement.crouchWalk;	// しゃがみ歩行
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
		_playerState.absorbState != PLAYER_ABSORB_STATE::NONE ||
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
			_oldPlayerState.absorbState	  != _playerState.absorbState	||	// 吸収攻撃状態
			_oldPlayerState.combatState   != _playerState.combatState);		// 戦闘状態
}

// アニメーションが終了したか
bool PlayerBase::IsAnimationFinished()
{
	// アニメーションマネージャーの取得
	AnimManager* animManager = GetAnimManager();

	if(animManager)
	{
		// 現在のアニメーションが終了しているか
		return animManager->IsAnimationFinished();
	}
}