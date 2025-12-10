#include "SurfacePlayer.h"
#include "ApplicationMain.h"

SurfacePlayer::SurfacePlayer()
{
	//_iHandle = MV1LoadModel("res/base/SDChar/SDChar.mv1");

	// モデルデータのロード（テクスチャも読み込まれる）
	_iHandle = MV1LoadModel("res/base/SDChar/SDChar.mv1");
	// 3Dモデルの1番目のアニメーションをアタッチする
	_iAttachIndex = MV1AttachAnim(_iHandle, 1, -1, FALSE);
	// アタッチしたアニメーションの総再生時間を取得する
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	// 再生時間の初期化
	_fPlayTime = 0.0f;
	// 位置の初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);		// キャラモデルはデフォルトで-Z方向を向いている

	// 腰位置の設定
	_colSubY = 40.f;

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

	// 処理前のステータスを保存しておく
	_eOldStatus = _eStatus;

	// 移動方向を決める
	_vMove = { 0,0,0 };
	_fMoveSpeed = 6.0f;
	if(_key & PAD_INPUT_DOWN) { _vMove.z = 1; }
	if(_key & PAD_INPUT_UP) { _vMove.z = -1; }
	if(_key & PAD_INPUT_LEFT) { _vMove.x = 1; }
	if(_key & PAD_INPUT_RIGHT) { _vMove.x = -1; }

	_vPos = VAdd(_vPos, VScale(_vMove, _fMoveSpeed));

	// 移動量をそのままキャラの向きにする
	if(VSize(_vMove) > 0.f)	// 移動していない時は無視するため
	{		
		_vDir = _vMove;
		_eStatus = CHARA_STATUS::WALK;
	}
	else
	{
		_eStatus = CHARA_STATUS::WAIT;
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
			case CHARA_STATUS::WAIT:
			{
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "idle"), -1, FALSE);
				break;
			}
			case CHARA_STATUS::WALK:
			{
				_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "run"), -1, FALSE);
				break;
			}
		}

		// アタッチしたアニメーションの総再生時間を取得する
		_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);

		// 再生時間を初期化
		_fPlayTime = 0.0f;
	}

	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if(_fPlayTime >= _fTotalTime)
	{
		_fPlayTime = 0.0f;
	}

	return true;
}

bool SurfacePlayer::Render()
{
	// いったんとりあえずこれ
	{
		//// 3D基本設定
		//SetUseZBuffer3D(TRUE);
		//SetWriteZBuffer3D(TRUE);
		//SetUseBackCulling(TRUE);

		// カメラ設定更新
		//SetCameraPositionAndTarget_UpVecY(_cam._vPos, _cam._vTarget);
		//SetCameraNearFar(_cam._clipNear, _cam._clipFar);

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

		// ばかなりた情報表示
		{
			int x = 0, y = 0, size = 16;
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 0, 0), "Player:"); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
		}
	}

	return true;
}