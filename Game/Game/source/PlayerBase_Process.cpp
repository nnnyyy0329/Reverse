// 担当 : 成田

#include "PlayerBase.h"

namespace HitConfig
{
	const float HIT_SPEED = 5.0f;	// 被弾時の吹き飛び速度
	const float HIT_DECAY = 0.9f;	// 被弾時の吹き飛び減衰率
	const float HIT_TIME = 15.0f;	// 被弾時間
}

// 共通関数呼び出し😊
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

	// 攻撃中は移動入力を受け付けない
	if(!IsAttacking())
	{
		/*if(_key & PAD_INPUT_DOWN) { _vMove.z = 1; }
		if(_key & PAD_INPUT_UP) { _vMove.z = -1; }
		if(_key & PAD_INPUT_LEFT) { _vMove.x = 1; }
		if(_key & PAD_INPUT_RIGHT) { _vMove.x = -1; }*/
		
		// しゃがみ中かどうかで移動速度を変える
		if(_bIsCrouching)
		{
			_fMoveSpeed = _playerConfig.crouchMoveSpeed;
		}
		else
		{
			_fMoveSpeed = _playerConfig.normalMoveSpeed;
		}

		// アナログ入力による移動
		if(abs(_lx) > _analogMin || abs(_ly) > _analogMin)
		{
			// カメラの向いている方向のベクトル
			VECTOR cameraForward = VGet(cos(_cameraAngle), 0.0f, sin(_cameraAngle));
			VECTOR cameraRight = VGet(cos(_cameraAngle + DX_PI_F / 2.0f), 0.0f, sin(_cameraAngle + DX_PI_F / 2.0f));

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

			// ダッシュ入力
			if(_key & PAD_INPUT_5)
			{
				_fMoveSpeed += _playerConfig.dashMoveSpeed;
			}
		}

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

// 状態変化アニメーション処理
void PlayerBase::ProcessStatusAnimation()
{
	// 攻撃中
	if(IsAttacking())
	{
		// アニメーション再生処理のみ
		ProcessPlayAnimation();

		// 処理後の攻撃中のステータスを保存
		_eOldPlayerStatus = _ePlayerStatus;

		return;
	}

	_eOldPlayerStatus = _ePlayerStatus;	// 処理前のステータスを保存しておく

	// 空中ならジャンプステータス
	if(!_bIsStanding)
	{
		if(_fVelY > 0.0f) // 上昇中
		{
			_ePlayerStatus = PLAYER_STATUS::JUMP_UP;
		}
		else // 下降中
		{
			_ePlayerStatus = PLAYER_STATUS::JUMP_DOWN;
		}
	}
	// 地上にいる場合
	else
	{
		bool isWalking = VSize(VGet(_vMove.x, 0.0f, _vMove.z)) > 0.0f;
		bool isRunning = (_key & PAD_INPUT_5) != 0;

		if(isWalking && isRunning) // 走っているなら
		{
			_vDir = _vMove;							// 移動方向を向く
			_ePlayerStatus = PLAYER_STATUS::RUN;	// 走行
		}
		else if(isWalking) // 歩いているなら
		{
			_vDir = _vMove;							// 移動方向を向く
			_ePlayerStatus = PLAYER_STATUS::WALK;	// 歩行
		}
		else // 止まっているなら
		{
			_ePlayerStatus = PLAYER_STATUS::WAIT;	// 待機
		}
	}

	// アニメーション再生処理
	ProcessPlayAnimation();
}

// アニメーション処理
void PlayerBase::ProcessPlayAnimation()
{
	// ステータスが変化していなければ処理しない
	if(_eOldPlayerStatus == _ePlayerStatus){ return; }

	// アニメーションマネージャーの取得
	AnimManager* animManager = GetAnimManager();
	if(animManager == nullptr){ return; }

	const char* animName = nullptr;
	float blendTime = 1.0f;
	int loopCnt = 0;

	// ステータスに応じた設定
	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::WAIT:		
			animName = _playerAnim.wait;
			blendTime = 1.0f;
			loopCnt = 0;
			break;

		case PLAYER_STATUS::WALK:		
			animName = _playerAnim.walk;
			blendTime = 1.0f;
			loopCnt = 0;
			break;

		case PLAYER_STATUS::RUN:		
			animName = _playerAnim.run;
			blendTime = 1.0f;
			loopCnt = 0;
			break;

		case PLAYER_STATUS::JUMP_UP:	
			animName = _playerAnim.jumpUp;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::JUMP_DOWN:		
			animName = _playerAnim.jumpDown;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::CROUCH_WAIT:	
			animName = _playerAnim.crouchWait;
			blendTime = 1.0f;
			loopCnt = 0;
			break;

		case PLAYER_STATUS::CROUCH_WALK:	
			animName = _playerAnim.crouchWalk;
			blendTime = 1.0f;
			loopCnt = 0;
			break;

		case PLAYER_STATUS::FIRST_ATTACK:	
			animName = _playerAnim.firstAttack;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::SECOND_ATTACK:
			animName = _playerAnim.secondAttack;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::THIRD_ATTACK:	
			animName = _playerAnim.thirdAttack;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::FOURTH_ATTACK:
			animName = _playerAnim.fourthAttack;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::FIFTH_ATTACK:
			animName = _playerAnim.fifthAttack;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::HIT:			
			animName = _playerAnim.hit;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::DODGE:			
			animName = _playerAnim.dodge;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		case PLAYER_STATUS::DEATH:			
			animName = _playerAnim.death;
			blendTime = 1.0f;
			loopCnt = 1;
			break;

		default:
			return;
	}

	if(animName == nullptr || strlen(animName) == 0)
	{
		return; // アニメーション名が無効な場合は処理しない
	}

	// アニメーション切り替え
	animManager->ChangeAnimationByName(animName, blendTime, loopCnt);
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
		_vCollisionTop = VAdd(modelPos, VGet(0, 70.0f, 0));
		_vCollisionBottom = VAdd(modelPos, VGet(0, 10.0f, 0));
	}
	else // しゃがみ中なら
	{
		_vCollisionTop = VAdd(modelPos, VGet(0, 35.0f, 0));
		_vCollisionBottom = VAdd(modelPos, VGet(0, 5.0f, 0));
	}
}

// 被弾処理
void PlayerBase::ProcessHit()
{
	if(_ePlayerStatus != PLAYER_STATUS::HIT){ return; }	// 被弾状態でない場合は処理しない

	_fHitTime -= 1.0f;	// 被弾時間を減らす

	// 被弾時間が終了したら通常状態に戻る
	if(_fHitTime <= 0.0f)
	{
		_ePlayerStatus = PLAYER_STATUS::WAIT;
		_fHitSpeed = 0.0f;
		_vHitDir = VGet(0, 0, 0);
		return;
	}

	// 吹き飛び処理
	if(_fHitSpeed > 0.0f)
	{
		// のけぞり方向に移動
		_vPos = VAdd(_vPos, VScale(_vHitDir, _fHitSpeed));

		// 吹き飛び速度減衰
		_fHitSpeed *= HitConfig::HIT_DECAY;

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

// 死亡処理
void PlayerBase::ProcessDeath()
{
	// 体力が0以下なら死亡処理
	if(_fLife <= 0.0f && _ePlayerStatus != PLAYER_STATUS::DEATH)
	{
		_ePlayerStatus = PLAYER_STATUS::DEATH;


	}
}

// デバッグ処理
void PlayerBase::ProcessDebug()
{
	if(_trg & PAD_INPUT_5)
	{
		_fLife -= 5.0f;
	}
}