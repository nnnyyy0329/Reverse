// 担当 : 成田

#include "SurfacePlayer.h"
#include "ApplicationMain.h"

// アクション関係Process呼び出し用関数
void SurfacePlayer::CallProcess()
{
	// プレイヤー移動処理
	ProcessMovePlayer();

	// ジャンプ処理
	ProcessJump();

	// 着地処理
	ProcessStanding();

	// しゃがみ処理
	//ProcessCrouch();

	// ステータスに応じたアニメーション処理
	ProcessStatusAnimation();

	// デバッグ用の処理
	ProcessDebug();
}

// プレイヤー移動処理
void SurfacePlayer::ProcessMovePlayer()
{
	// 移動方向を決める
	_vMove = { 0,0,0 };

	// 攻撃中は移動入力を受け付けない
	if(!IsAttacking())
	{
		// しゃがみ中かどうかで移動速度を変える
		if(_bIsCrouching){ _fMoveSpeed = 3.0f; }
		else{ _fMoveSpeed = 6.0f; }

		if(_key & PAD_INPUT_DOWN) { _vMove.z = 1; }
		if(_key & PAD_INPUT_UP) { _vMove.z = -1; }
		if(_key & PAD_INPUT_LEFT) { _vMove.x = 1; }
		if(_key & PAD_INPUT_RIGHT) { _vMove.x = -1; }

		// 移動量を正規化
		float len = VSize(_vMove);
		if(len > 0.0f)
		{
			_vMove.x /= len;	// 正規化
			_vMove.y /= len;	// 正規化
		}

		_vPos = VAdd(_vPos, VScale(_vMove, _fMoveSpeed));	// 移動速度を掛けて移動
	}

	// カプセルに座標を対応
	if(!_bIsCrouching) // しゃがみ中じゃないなら
	{
		_vCollisionTop = VAdd(_vPos, VGet(0, 70.0f, 0));	// キャラの高さ分を足す
		_vCollisionBottom = VAdd(_vPos, VGet(0, 10.0f, 0));	// 腰位置分を足す
	}
	else // しゃがみ中なら
	{
		_vCollisionTop = VAdd(_vPos, VGet(0, 35.0f, 0));
		_vCollisionBottom = VAdd(_vPos, VGet(0, 5.0f, 0));
	}
}

// ステータスに応じたアニメーション処理
void SurfacePlayer::ProcessStatusAnimation()
{
	// 処理前のステータスを保存しておく
	_eOldPlayerStatus = _ePlayerStatus;

	// 攻撃中
	if(IsAttacking())
	{
		// アニメーション再生処理のみ
		ProcessPlayAnimation();
		return;
	}

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
	else
	{
		// しゃがみ中かどうか
		if(_bIsCrouching)
		{
			// しゃがみ中
			if(VSize(VGet(_vMove.x, 0.0f, _vMove.z)) > 0.0f) // 歩いているかどうか
			{
				_vDir = _vMove;									// 移動方向を向く
				_ePlayerStatus = PLAYER_STATUS::CROUCH_WALK;	// しゃがみ歩行
			}
			else // 止まっている
			{
				_ePlayerStatus = PLAYER_STATUS::CROUCH_WAIT;	// しゃがみ待機
			}
		}
		// 通常移動
		else
		{
			// 通常 
			if(VSize(VGet(_vMove.x, 0.0f, _vMove.z)) > 0.0f) // 歩いているかどうか
			{
				_vDir = _vMove;							// 移動方向を向く
				_ePlayerStatus = PLAYER_STATUS::WALK;	// 歩行
			}
			else // 止まっている
			{
				_ePlayerStatus = PLAYER_STATUS::WAIT;	// 待機
			}
		}
	}

	// アニメーション再生処理
	ProcessPlayAnimation();
}

// アニメーション再生処理
void SurfacePlayer::ProcessPlayAnimation()
{
	// ステータスが変わっていないか？
	if(_eOldPlayerStatus == _ePlayerStatus)
	{
		// 再生時間を進める
		_fPlayTime += 0.5f;
	}
	else
	{
		// アニメーションがアタッチされていたら、デタッチする
		if(_iAttachIndex != -1)
		{
			MV1DetachAnim(_iHandle, _iAttachIndex);
			_iAttachIndex = -1;
		}

		// ステータスに合わせてアニメーションのアタッチ
		switch(_ePlayerStatus)
		{
			case PLAYER_STATUS::WAIT:	// 待機
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "idle"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::WALK:	// 歩行
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "run"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::JUMP_UP: // ジャンプ上昇
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "jump_up"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::JUMP_DOWN: // ジャンプ下降
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "jump_down"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::CROUCH_WAIT:	// しゃがみ待機
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "crouch_idle"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::CROUCH_WALK:	// しゃがみ歩行
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "crouch"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::FIRST_ATTACK:	// 攻撃1
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_01"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::SECOND_ATTACK:	// 攻撃2
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_02"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::THIRD_ATTACK:	// 攻撃3
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_03"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::HIT:			// 被弾
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "death"), -1, FALSE);
				break;
			}
			case PLAYER_STATUS::DEATH:	// 死亡
			{
				_iAttachIndex = -1;
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "death"), -1, FALSE);
				break;
			}
		}

		// アタッチしたアニメーションの総再生時間を取得する
		if(_iAttachIndex != -1)
		{
			_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
		}		
		
		_fPlayTime = 0.0f;
	}

	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if(_iAttachIndex != -1 && _fPlayTime >= _fTotalTime)
	{
		_fPlayTime = 0.0f;
	}
}

// 着地処理
void SurfacePlayer::ProcessStanding()
{
	// 重力を加算する
	if(!_bIsStanding)
	{
		_fVelY += _fGravity;	// 重力加速度を加算
	}

	_vPos.y += _fVelY;	// Y方向の速度分、位置を更新

	// 地面との衝突
	if(_vPos.y <= 0.0f) // 地面の高さが０の場合
	{
		_vPos.y = 0.0f;			// 地面の高さに合わせる
		_fVelY = 0.0f;			// Y方向の速度を０にする
		_bIsStanding = true;	// 着地フラグを立てる	
		_bIsJumping = false;	// ジャンプ中フラグを下ろす
	}
	else
	{
		_bIsStanding = false;	// 空中
	}
}

// ジャンプ処理
void SurfacePlayer::ProcessJump()
{
	// しゃがみ中はジャンプできない
	if(_bIsCrouching){ return; }

	// 攻撃中はジャンプできない
	if(IsAttacking()){ return; }

	// ジャンプボタンが押されたら
	if(_trg & PAD_INPUT_A && _ePlayerStatus != PLAYER_STATUS::JUMP_UP)	// Zボタン
	{
		// ジャンプ中でなく、着地しているなら
		if(!_bIsJumping && _bIsStanding)
		{
			// ジャンプ開始
			_fVelY = 10.f;			// ジャンプ初速を設定
			_bIsJumping = true;		// ジャンプ中フラグを立てる
			_bIsStanding = false;	// 着地フラグを下ろす
		}
	}
}

// しゃがみ処理
void SurfacePlayer::ProcessCrouch()
{
	// 空中ではしゃがめない
	if(_bIsStanding == false){ return; }

	// 攻撃中はジャンプできない
	if(IsAttacking()){ return; }

	// しゃがみボタンが押されたら
	if(_trg & PAD_INPUT_B)
	{
		// しゃがみ開始フラグが立っておらず、しゃがみステータスでなければ
		if(!_bIsStartCrouch && _ePlayerStatus != PLAYER_STATUS::CROUCH_WAIT)
		{
			// しゃがみ開始
			const bool next = !_bIsStartCrouch;		// しゃがみ開始フラグを反転

			_bIsStartCrouch = next;					// しゃがみ開始フラグを立てる
			_bIsCrouching = next;					// しゃがみフラグを立てる
			_ePlayerStatus = PLAYER_STATUS::CROUCH_WAIT;	// ステータスをしゃがみにする
		}
		else // しゃがみ解除
		{
			_bIsCrouching = false;		// しゃがみフラグを下ろす	
			_bIsStartCrouch = false;	// しゃがみ開始フラグを下ろす
		}
	}
}

// 死亡処理
void SurfacePlayer::ProcessDeath()
{
	// 体力が0以下ならステータスを死亡に変更
	if(_fLife <= 0){ _ePlayerStatus = PLAYER_STATUS::DEATH; }

	// 死亡時のアニメーション
	if(_ePlayerStatus == PLAYER_STATUS::DEATH)
	{
		// アニメーション再生処理
		ProcessPlayAnimation();
	}
}

// デバッグ用関数
void SurfacePlayer::ProcessDebug()
{
	// 体力減少
	{
		if(_trg & PAD_INPUT_8){ _fLife -= 5.0f; }
	}
}