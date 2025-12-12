#include "SurfacePlayer.h"
#include "ApplicationMain.h"

namespace
{
	const float GRAVITY = -0.6f;	// 重力加速度
}

SurfacePlayer::SurfacePlayer()
{
	// モデルデータのロード（テクスチャも読み込まれる）
	_iHandle = MV1LoadModel("res/base/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 1, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	// 位置の初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);

	// 基礎ステータスの初期化
	_eStatus = CHARA_STATUS::NONE;
	_fMoveSpeed = 0.0f;
	_fDirSpeed = 0.0f;
	_fLife = 100.0f;
	_fGravity = GRAVITY;

	// 固有変数の初期化
	_fVelY = 0.0f;
	_bIsJumping = false;
	_bIsStanding = true;
	_bIsCrouching = false;
	_bIsStartCrouch = false;

	// カプセルコリジョンの設定
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);	// 上端
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);	// 下端
	_fCollisionR = 20.f;						// 半径

	// 腰位置の設定
	_colSubY = 40.f;
	_bViewCollision = false;
}

SurfacePlayer::~SurfacePlayer()
{
	MV1DeleteModel(_iHandle);
}

bool SurfacePlayer::Initialize()
{
	return true;
}

bool SurfacePlayer::Terminate()
{
	return true;
}

bool SurfacePlayer::Process()
{
	// 体力が0以下なら
	if(_fLife <= 0.f) { _eStatus = CHARA_STATUS::DEATH; return false; }

	if(_key & PAD_INPUT_8){ _fLife -= 5.0f; }

	// いったんとりあえずこれ
	{
	//	// 処理前のステータスを保存しておく
	//	CHARA_STATUS oldStatus = _eStatus;
	//	if(_key & PAD_INPUT_8) {	// W
	//		// カメラ位置（注目位置もXZスライド）
	//		float sx = _cam._vPos.x - _cam._vTarget.x;
	//		float sz = _cam._vPos.z - _cam._vTarget.z;
	//		float camrad = atan2(sz, sx);

	//		// 移動方向を決める(左スティック)
	//		{
	//			_vMove = { 0,0,0 };
	//			float mvSpeed = 2.f;
	//			float length = sqrt(_lx * _lx + _ly * _ly);
	//			float rad = atan2(_lx, _ly);
	//			if(length < _analogMin) {
	//				// 入力が小さかったら動かなかったことにする
	//				length = 0.f;
	//			}
	//			else {
	//				length = mvSpeed;
	//			}
	//			// vをrad分回転させる
	//			_vMove.x = cos(rad + camrad) * length;
	//			_vMove.z = sin(rad + camrad) * length;
	//			// vの分移動
	//			_cam._vPos = VAdd(_cam._vPos, _vMove);
	//			_cam._vTarget = VAdd(_cam._vTarget, _vMove);
	//		}
	//		// 距離、ターゲットの高さ変更（右スティック）
	//		{
	//			float sx = _cam._vPos.x - _cam._vTarget.x;
	//			float sz = _cam._vPos.z - _cam._vTarget.z;
	//			float rad = atan2(sz, sx);
	//			float length = sqrt(sz * sz + sx * sx);
	//			if(_rx < -_analogMin) { length -= 1.f; }
	//			if(_rx > _analogMin) { length += 1.f; }
	//			_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
	//			_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;
	//			// Y位置
	//			if(_ry > _analogMin) { _cam._vTarget.y -= 1.f; }
	//			if(_ry < -_analogMin) { _cam._vTarget.y += 1.f; }
	//		}
	//	}
	//	else {
	//		// キャラ移動(カメラ設定に合わせて)

	//		// カメラの向いている角度を取得
	//		float sx = _cam._vPos.x - _cam._vTarget.x;
	//		float sz = _cam._vPos.z - _cam._vTarget.z;
	//		float camrad = atan2(sz, sx);
	//		// 移動方向を決める
	//		VECTOR _vMove = { 0,0,0 };
	//		float mvSpeed = 6.f;
	//		// アナログ左スティック用
	//		float length = sqrt(_lx * _lx + _ly * _ly);
	//		float rad = atan2(_lx, _ly);
	//		if(length < _analogMin) {
	//			// 入力が小さかったら動かなかったことにする
	//			length = 0.f;
	//		}
	//		else {
	//			length = mvSpeed;
	//		}
	//		// vをrad分回転させる
	//		_vMove.x = cos(rad + camrad) * length;
	//		_vMove.z = sin(rad + camrad) * length;
	//		// 移動前の位置を保存
	//		VECTOR oldvPos = _vPos;
	//		VECTOR oldv = _vMove;
	//		// 移動量をそのままキャラの向きにする
	//		if(VSize(_vMove) > 0.f) {		// 移動していない時は無視するため
	//			_vDir = _vMove;
	//			_eStatus = CHARA_STATUS::WALK;
	//		}
	//		else {
	//			_eStatus = CHARA_STATUS::WAIT;
	//		}
	//		//// カメラ操作を行う（右スティック）
	//		//{
	//		//	// Y軸回転
	//		//	float sx = _cam._vPos.x - _cam._vTarget.x;
	//		//	float sz = _cam._vPos.z - _cam._vTarget.z;
	//		//	float rad = atan2(sz, sx);
	//		//	float length = sqrt(sz * sz + sx * sx);
	//		//	if(_rx > _analogMin) { rad -= 0.05f; }
	//		//	if(_rx < -_analogMin) { rad += 0.05f; }
	//		//	_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
	//		//	_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;

	//		//	// Y位置
	//		//	if(_ry > _analogMin) { _cam._vPos.y -= 5.f; }
	//		//	if(_ry < -_analogMin) { _cam._vPos.y += 5.f; }
	//		//}
	//	}

	//	// ステータスが変わっていないか？
	//	if(oldStatus == _eStatus) {
	//		// 再生時間を進める
	//		_fPlayTime += 0.5f;
	//	}
	//	else {
	//		// アニメーションがアタッチされていたら、デタッチする
	//		if(_iAttachIndex != -1) {
	//			MV1DetachAnim(_iHandle, _iAttachIndex);
	//			_iAttachIndex = -1;
	//		}
	//		// ステータスに合わせてアニメーションのアタッチ
	//		switch(_eStatus) {
	//			case CHARA_STATUS::WAIT:
	//				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "idle"), -1, FALSE);
	//				break;
	//			case CHARA_STATUS::WALK:
	//				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "run"), -1, FALSE);
	//				break;
	//		}
	//		// アタッチしたアニメーションの総再生時間を取得する
	//		_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	//		// 再生時間を初期化
	//		_fPlayTime = 0.0f;
	//	}

	//	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	//	if(_fPlayTime >= _fTotalTime) {
	//		_fPlayTime = 0.0f;
	//	}
	}

	// プレイヤー移動処理
	MovePlayer();

	// ジャンプ処理
	JumpProcess();

	// 着地処理
	StandingProcess();

	// しゃがみ処理
	CrouchProcess();

	// ステータスに応じたアニメーション処理
	StatusAnimationProcess();


	return true;
}

bool SurfacePlayer::Render()
{
	// 体力が0以下なら
	if(_fLife <= 0.f) { _eStatus = CHARA_STATUS::DEATH; return false; }

	// いったんとりあえずこれ
	{
		// 0,0,0を中心に線を引く
		{
			float linelength = 1000.f;
			VECTOR v = { 0, 0, 0 };
			DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
			DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
			DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
		}

		// 再生時間をセットする
		MV1SetAttachAnimTime(_iHandle, _iAttachIndex, _fPlayTime);

		// モデルを描画する
		{
			// 位置
			MV1SetPosition(_iHandle, _vPos);

			// 向きからY軸回転を算出
			VECTOR vRot = { 0,0,0 };
			vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);	// モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
			MV1SetRotationXYZ(_iHandle, vRot);

			// 描画
			MV1DrawModel(_iHandle);

			// コリジョン判定用ラインの描画
			if(_bViewCollision)
			{
				DrawLine3D(VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)), GetColor(255, 0, 0));
			}
		}

		// プレイヤー情報表示
		{
			int x = 0, y = 0, size = 16;
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 0, 0), "Player:"); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
		}
	}

	// デバッグ用
	DrawCapsuleCollision();	// カプセルコリジョンを描画
	DrawStatus();			// ステータスを描画

	return true;
}

// プレイヤー移動処理
void SurfacePlayer::MovePlayer()
{
	// 処理前のステータスを保存しておく
	_eOldStatus = _eStatus;

	// 移動方向を決める
	_vMove = { 0,0,0 };

	// しゃがみ中かどうかで移動速度を変える
	if(_bIsCrouching){_fMoveSpeed = 3.0f;}
	else{_fMoveSpeed = 6.0f;}

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
void SurfacePlayer::StatusAnimationProcess()
{
	// 空中ならジャンプステータス
	if(!_bIsStanding)
	{
		if(_fVelY > 0.0f) // 上昇中
		{
			_eStatus = CHARA_STATUS::JUMP_UP;
		}
		else // 下降中
		{
			_eStatus = CHARA_STATUS::JUMP_DOWN;
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
				_vDir = _vMove;							// 移動方向を向く
				_eStatus = CHARA_STATUS::CROUCH_WALK;	// しゃがみ歩行
			}
			else // 止まっている
			{
				_eStatus = CHARA_STATUS::CROUCH_WAIT;	// しゃがみ待機
			}
		}
		else // しゃがみ中でない
		{
			// 通常 
			if(VSize(VGet(_vMove.x, 0.0f, _vMove.z)) > 0.0f) // 歩いているかどうか
			{
				_vDir = _vMove;					// 移動方向を向く
				_eStatus = CHARA_STATUS::WALK;	// 歩行
			}
			else // 止まっている
			{
				_eStatus = CHARA_STATUS::WAIT;	// 待機
			}
		}
	}

	// ステータスが変わっていないか？
	if(_eOldStatus == _eStatus)
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
		switch(_eStatus)
		{
			case CHARA_STATUS::WAIT:	// 待機
			{
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "idle"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::WALK:	// 歩行
			{
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "run"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::JUMP_UP: // ジャンプ上昇
			{
				// _iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "jump_up"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::JUMP_DOWN: // ジャンプ下降
			{
				// _iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "jump_down"), -1, FALSE);
				break;
			}	
			case CHARA_STATUS::CROUCH_WAIT:	// しゃがみ待機
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "crouch_idle"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::CROUCH_WALK:	// しゃがみ歩行
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "crouch"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::FIRST_ATTACK:	// 攻撃1
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_01"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::SECOND_ATTACK:	// 攻撃2
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_02"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::THIRD_ATTACK:	// 攻撃3
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "attack_03"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::DEATH:	// 死亡
			{
				//_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "death"), -1, FALSE);
				break;
			}
		}

		// アタッチしたアニメーションの総再生時間を取得する
		_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
		_fPlayTime = 0.0f;
	}

	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if(_fPlayTime >= _fTotalTime)
	{
		_fPlayTime = 0.0f;
	}
}

// 着地処理
void SurfacePlayer::StandingProcess()
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
void SurfacePlayer::JumpProcess()
{
	// しゃがみ中はジャンプできない
	if(_bIsCrouching){ return; } 

	// ジャンプボタンが押されたら
	if(_trg & PAD_INPUT_A && _eStatus != CHARA_STATUS::JUMP_UP)	// Zボタン
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
void SurfacePlayer::CrouchProcess()
{
	// 空中ではしゃがめない
	if(_bIsStanding == false){ return; } 

	// しゃがみボタンが押されたら
	if(_trg & PAD_INPUT_B)
	{
		// しゃがみ開始フラグが立っておらず、しゃがみステータスでなければ
		if(!_bIsStartCrouch && _eStatus != CHARA_STATUS::CROUCH_WAIT)
		{
			// しゃがみ開始
			const bool next = !_bIsStartCrouch;		// しゃがみ開始フラグを反転

			_bIsStartCrouch = next;					// しゃがみ開始フラグを立てる
			_bIsCrouching = next;					// しゃがみフラグを立てる
			_eStatus = CHARA_STATUS::CROUCH_WAIT;	// ステータスをしゃがみにする
		}
		else // しゃがみ解除
		{
			_bIsCrouching = false;		// しゃがみフラグを下ろす	
			_bIsStartCrouch = false;	// しゃがみ開始フラグを下ろす
		}
	}
}

/* ーーーーーーーーーーーーーデバッグ表示ーーーーーーーーーーーーー */

// カプセルコリジョン描画
void SurfacePlayer::DrawCapsuleCollision()
{
	DrawCapsule3D
	(
		_vCollisionTop, 
		_vCollisionBottom, 
		_fCollisionR,
		8,
		GetColor(0, 255, 0),
		GetColor(0, 0, 255),
		TRUE
	);

	// カプセルコリジョン座標表示
	{
		int x = 400, y = 0, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(255, 0, 0), "Capsule Collision:"); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Top     = (%5.2f, %5.2f, %5.2f)", _vCollisionTop.x, _vCollisionTop.y, _vCollisionTop.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Bottom  = (%5.2f, %5.2f, %5.2f)", _vCollisionBottom.x, _vCollisionBottom.y, _vCollisionBottom.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Radius  = %5.2f", _fCollisionR); y += size;
	}
}

// ステータス描画
void SurfacePlayer::DrawStatus()
{
	const char* status = "";
	switch(_eStatus)
	{
		case CHARA_STATUS::NONE:			// 初期状態
			status = "NONE";
			break;
		case CHARA_STATUS::WAIT:			// 待機
			status = "WAIT";
			break;
		case CHARA_STATUS::WALK:			// 歩行
			status = "WALK";
			break;
		case CHARA_STATUS::FIRST_ATTACK:	// 攻撃1
			status = "FIRST_ATTACK";
			break;
		case CHARA_STATUS::SECOND_ATTACK:	// 攻撃2
			status = "SECOND_ATTACK";
			break;
		case CHARA_STATUS::THIRD_ATTACK:	// 攻撃3
			status = "THIRD_ATTACK";
			break;
		case CHARA_STATUS::JUMP_UP:			// ジャンプ上昇
			status = "JUMP_UP";
			break;
		case CHARA_STATUS::JUMP_DOWN:		// ジャンプ下降
			status = "JUMP_DOWN";
			break;
		case CHARA_STATUS::CROUCH_WAIT:		// しゃがみ待機
			status = "CROUCH_WAIT";
			break;
		case CHARA_STATUS::CROUCH_WALK:		// しゃがみ歩行
			status = "CROUCH_WALK";
			break;
		case CHARA_STATUS::DEATH:			// 死亡
			status = "DEATH";	
			break;
		default:
			status = "UNKNOWN";
			break;
	}
	
	DrawFormatString(0, 40, GetColor(255, 0, 0), "Player Status: %s", status);
}