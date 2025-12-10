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
	// マップ
	_handleSkySphere = MV1LoadModel("res/base/SkySphere/skysphere.mv1");
#if 0
	// ダンジョン
	_handleMap = MV1LoadModel("res/base/Dungeon/Dungeon.mv1");
	_frameMapCollision = MV1SearchFrame(_handleMap, "dungeon_collision");
#else
	// フィールド
	_handleMap = MV1LoadModel("res/base/Ground/Ground.mv1");
	_frameMapCollision = MV1SearchFrame(_handleMap, "ground_navmesh");
#endif
	// コリジョン情報の生成
	MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
	// コリジョンのフレームを描画しない設定
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
	// 腰位置の設定
	_colSubY = 40.f;
	// カメラの設定（わかりやすい位置に）
	_cam._vPos = VGet(0, 90.f, -300.f);
	_cam._vTarget = VGet(0, 60, 0);
	_cam._clipNear = 2.f;
	_cam._clipFar = 10000.f;
	// その他初期化
	_bViewCollision = TRUE;


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
		//int key = ApplicationMain::GetInstance()->GetKey();
		//int trg = ApplicationMain::GetInstance()->GetTrg();

		// 処理前のステータスを保存しておく
		CHARA_STATUS oldStatus = _eStatus;

		//// アナログスティック対応
		//DINPUT_JOYSTATE di;
		//GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
		//float lx, ly, rx, ry;	// 左右アナログスティックの座標
		//float analogMin = 0.3f;	// アナログ閾値
		//// Logicoolパッドの場合
		//lx = (float)di.X / 1000.f; ly = (float)di.Y / 1000.f;
		//rx = (float)di.Z / 1000.f; ry = (float)di.Rz / 1000.f;


		if(_key & PAD_INPUT_8) {	// W
			// カメラ位置（注目位置もXZスライド）
			float sx = _cam._vPos.x - _cam._vTarget.x;
			float sz = _cam._vPos.z - _cam._vTarget.z;
			float camrad = atan2(sz, sx);

			// 移動方向を決める(左スティック)
			{
				VECTOR v = { 0,0,0 };
				float mvSpeed = 2.f;
				float length = sqrt(_lx * _lx + _ly * _ly);
				float rad = atan2(_lx, _ly);
				if(length < _analogMin) {
					// 入力が小さかったら動かなかったことにする
					length = 0.f;
				}
				else {
					length = mvSpeed;
				}

				// vをrad分回転させる
				v.x = cos(rad + camrad) * length;
				v.z = sin(rad + camrad) * length;

				// vの分移動
				_cam._vPos = VAdd(_cam._vPos, v);
				_cam._vTarget = VAdd(_cam._vTarget, v);
			}


			// 距離、ターゲットの高さ変更（右スティック）
			{
				float sx = _cam._vPos.x - _cam._vTarget.x;
				float sz = _cam._vPos.z - _cam._vTarget.z;
				float rad = atan2(sz, sx);
				float length = sqrt(sz * sz + sx * sx);
				if(_rx < -_analogMin) { length -= 1.f; }
				if(_rx > _analogMin) { length += 1.f; }
				_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
				_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;

				// Y位置
				if(_ry > _analogMin) { _cam._vTarget.y -= 1.f; }
				if(_ry < -_analogMin) { _cam._vTarget.y += 1.f; }
			}
		}
		else {
			// キャラ移動(カメラ設定に合わせて)

			// カメラの向いている角度を取得
			float sx = _cam._vPos.x - _cam._vTarget.x;
			float sz = _cam._vPos.z - _cam._vTarget.z;
			float camrad = atan2(sz, sx);

			// 移動方向を決める
			VECTOR v = { 0,0,0 };
			float mvSpeed = 6.f;

			// アナログ左スティック用
			float length = sqrt(_lx * _lx + _ly * _ly);
			float rad = atan2(_lx, _ly);
			if(length < _analogMin) {
				// 入力が小さかったら動かなかったことにする
				length = 0.f;
			}
			else {
				length = mvSpeed;
			}

			// vをrad分回転させる
			v.x = cos(rad + camrad) * length;
			v.z = sin(rad + camrad) * length;

			// 移動前の位置を保存
			VECTOR oldvPos = _vPos;
			VECTOR oldv = v;

			// コリジョン判定で引っかかった時に、escapeTbl[]順に角度を変えて回避を試みる
			float escapeTbl[] = {
				0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
			};
			for(int i = 0; i < sizeof(escapeTbl) / sizeof(escapeTbl[0]); i++) {
				// escapeTbl[i]の分だけ移動量v回転
				float escape_rad = DEG2RAD(escapeTbl[i]);
				v.x = cos(rad + camrad + escape_rad) * length;
				v.z = sin(rad + camrad + escape_rad) * length;

				// vの分移動
				_vPos = VAdd(_vPos, v);

				// 移動した先でコリジョン判定
				MV1_COLL_RESULT_POLY hitPoly;

				// 主人公の腰位置から下方向への直線
				hitPoly = MV1CollCheck_Line(_handleMap, _frameMapCollision,
					VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)));
				if(hitPoly.HitFlag) {
					// 当たった
					// 当たったY位置をキャラ座標にする
					_vPos.y = hitPoly.HitPosition.y;

					// キャラが上下に移動した量だけ、移動量を修正
					v.y += _vPos.y - oldvPos.y;

					// カメラも移動する
					_cam._vPos = VAdd(_cam._vPos, v);
					_cam._vTarget = VAdd(_cam._vTarget, v);

					// ループiから抜ける
					break;
				}
				else {
					// 当たらなかった。元の座標に戻す
					_vPos = oldvPos;
					v = oldv;
				}
			}

			// 移動量をそのままキャラの向きにする
			if(VSize(v) > 0.f) {		// 移動していない時は無視するため
				_vDir = v;
				_eStatus = CHARA_STATUS::WALK;
			}
			else {
				_eStatus = CHARA_STATUS::WAIT;
			}


			// カメラ操作を行う（右スティック）
			{
				// Y軸回転
				float sx = _cam._vPos.x - _cam._vTarget.x;
				float sz = _cam._vPos.z - _cam._vTarget.z;
				float rad = atan2(sz, sx);
				float length = sqrt(sz * sz + sx * sx);
				if(_rx > _analogMin) { rad -= 0.05f; }
				if(_rx < -_analogMin) { rad += 0.05f; }
				_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
				_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;

				// Y位置
				if(_ry > _analogMin) { _cam._vPos.y -= 5.f; }
				if(_ry < -_analogMin) { _cam._vPos.y += 5.f; }
			}



			// デバッグ機能
			if(_trg & PAD_INPUT_6) {
				_bViewCollision = !_bViewCollision;
			}
			if(_bViewCollision) {
				MV1SetFrameVisible(_handleMap, _frameMapCollision, TRUE);
			}
			else {
				MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
			}
		}

		// ステータスが変わっていないか？
		if(oldStatus == _eStatus) {
			// 再生時間を進める
			_fPlayTime += 0.5f;
		}
		else {
			// アニメーションがアタッチされていたら、デタッチする
			if(_iAttachIndex != -1) {
				MV1DetachAnim(_iHandle, _iAttachIndex);
				_iAttachIndex = -1;
			}
			// ステータスに合わせてアニメーションのアタッチ
			switch(_eStatus) {
				case CHARA_STATUS::WAIT:
					_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "idle"), -1, FALSE);
					break;
				case CHARA_STATUS::WALK:
					_iAttachIndex = MV1AttachAnim(_iHandle, MV1GetAnimIndex(_iHandle, "run"), -1, FALSE);
					break;
			}
			// アタッチしたアニメーションの総再生時間を取得する
			_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
			// 再生時間を初期化
			_fPlayTime = 0.0f;
		}

		// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
		if(_fPlayTime >= _fTotalTime) {
			_fPlayTime = 0.0f;
		}
	}

	return true;
}

bool SurfacePlayer::Render()
{
	// いったんとりあえずこれ
	{
		// 3D基本設定
		SetUseZBuffer3D(TRUE);
		SetWriteZBuffer3D(TRUE);
		SetUseBackCulling(TRUE);

		// ライト設定
		SetUseLighting(TRUE);

		#if 0	// 平行ライト
			SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
			ChangeLightTypeDir(VGet(-1, -1, 0));
		#endif
		#if 1	// ポイントライト
			SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
			ChangeLightTypePoint(VAdd(_vPos, VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
		#endif

		// カメラ設定更新
		SetCameraPositionAndTarget_UpVecY(_cam._vPos, _cam._vTarget);
		SetCameraNearFar(_cam._clipNear, _cam._clipFar);

		// 0,0,0を中心に線を引く
		{
			float linelength = 1000.f;
			VECTOR v = { 0, 0, 0 };
			DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
			DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
			DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
		}

		// カメラターゲットを中心に短い線を引く
		{
			float linelength = 10.f;
			VECTOR v = _cam._vTarget;
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
			vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);		// モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
			MV1SetRotationXYZ(_iHandle, vRot);
			// 描画
			MV1DrawModel(_iHandle);

			// コリジョン判定用ラインの描画
			if(_bViewCollision) {
				DrawLine3D(VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)), GetColor(255, 0, 0));
			}

		}

		// マップモデルを描画する
		{
			MV1DrawModel(_handleMap);
			MV1DrawModel(_handleSkySphere);
		}

		// カメラ情報表示
		{
			int x = 0, y = 0, size = 16;
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 0, 0), "Camera:"); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "  target = (%5.2f, %5.2f, %5.2f)", _cam._vTarget.x, _cam._vTarget.y, _cam._vTarget.z); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _cam._vPos.x, _cam._vPos.y, _cam._vPos.z); y += size;
			float sx = _cam._vPos.x - _cam._vTarget.x;
			float sz = _cam._vPos.z - _cam._vTarget.z;
			float length = sqrt(sz * sz + sx * sx);
			float rad = atan2(sz, sx);
			float deg = RAD2DEG(rad);
			DrawFormatString(x, y, GetColor(255, 0, 0), "  len = %5.2f, rad = %5.2f, deg = %5.2f", length, rad, deg); y += size;
		}

	}

	return true;
}