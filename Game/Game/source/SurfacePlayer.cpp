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
	_ePlayerStatus = PLAYER_STATUS::NONE;
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
	if(_fLife <= 0.f) { _ePlayerStatus = PLAYER_STATUS::DEATH; return false; }

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
	// プレイヤーが死亡しているなら
	if(_ePlayerStatus == PLAYER_STATUS::DEATH) { return false; }

	// モデル表示
	DrawModel();

	// デバッグ用
	DrawOther();			// その他の表示
	DrawCapsuleCollision();	// カプセルコリジョンを描画
	DrawStatus();			// ステータスを描画

	return true;
}