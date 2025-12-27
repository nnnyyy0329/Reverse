
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

#include "StageBase.h"
#include "CameraManager.h"
#include "SurfacePlayer.h"

bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	// オブジェクトの生成、初期設定
	{
		_player = std::make_shared<SurfacePlayer>();
		_player->Initialize();

		_stage = std::make_shared<StageBase>(3);

		_cameraManager = std::make_shared<CameraManager>();
		_cameraManager->SetTarget(_player);
	}

	return true;
}

bool ModeGame::Terminate() {
	base::Terminate();

	_player.reset();

	return true;
}

bool ModeGame::Process() {
	base::Process();
	/// 入力取得
	{
		int key = ApplicationMain::GetInstance()->GetKey();
		int trg = ApplicationMain::GetInstance()->GetTrg();
		auto analog = ApplicationMain::GetInstance()->GetAnalog();
		float lx = analog.lx;
		float ly = analog.ly;
		float rx = analog.rx;
		float ry = analog.ry;
		float analogMin = ApplicationMain::GetInstance()->GetAnalogMin();

		// プレイヤーに入力状態を渡す
		if (_player) {
			_player->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if (_cameraManager) {
			_cameraManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
	}

	// オブジェクトの更新
	{
		_player->Process();
		_stage->Process();
	}

	// 当たり判定
	{
		CheckCollisionPlayerMap();
	}


	_cameraManager->Process();// カメラ更新

	return true;
}

bool ModeGame::Render() {
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	SetUseLighting(TRUE);

#if 1	// 平行ライト
	SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
	ChangeLightTypeDir(VGet(-1, -1, 0));
#endif
#if 0	// ポイントライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypePoint(VAdd(_player->GetPos(), VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
#endif

	_cameraManager->SetUp();// カメラ設定更新
	
	// オブジェクトの描画
	{
		_player->Render();
		_stage->Render();
	}

	// デバッグ情報の描画
	{
	}

	return true;
}

// プレイヤーとマップの当たり判定
void ModeGame::CheckCollisionPlayerMap() {
	if (!_player || !_stage) return;

	VECTOR vCurrentPos = _player->GetPos();// 現在の位置
	VECTOR vOldPos = _player->GetOldPos();// 前フレームの位置

	// 移動ベクトルと移動量を計算
	VECTOR vMove = VSub(vCurrentPos, vOldPos);
	float moveLength = VSize(vMove);

	// 移動していないなら処理しない
	if (moveLength < 0.001f) {
		return;
	}

	// 移動角度を計算
	float moveRad = atan2(vMove.z, vMove.x);

	// escapeTbl[]順に角度を変えて回避を試みる
	float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};

	const auto& mapObjList = _stage->GetMapModelPosList();// ステージのマップオブジェクトリストを取得

	bool isLanded = false;// 地面が見つかったか

	for (int i = 0; i < sizeof(escapeTbl) / sizeof(escapeTbl[0]); i++) {
		// escapeTbl[]分だけ角度をずらす
		float escapeRad = DEG2RAD(escapeTbl[i]);
		float checkRad = moveRad + escapeRad;

		// 判定用の移動ベクトル
		VECTOR v;
		v.x = cos(checkRad) * moveLength;
		v.y = 0.0f;// Yは無視してXZ平面で判定
		v.z = sin(checkRad) * moveLength;

		VECTOR testPos = VAdd(vOldPos, v);// 判定用の位置(前フレームの位置 + ずらした移動量)

		// 判定用の線分の始点と終点を設定
		VECTOR lineStart = VAdd(testPos, VGet(0, _player->GetColSubY(), 0));// 始点：キャラの腰位置
		VECTOR lineEnd = VAdd(testPos, VGet(0, -1000.0f, 0));// 終点：かなり下の位置

		// 当たった中で最も高い地面を記録する
		float highestY = -99999.0f;
		bool hitAnyObj = false;

		// 全てのマップオブジェクトと当たり判定を行う
		for (const auto& obj : mapObjList) {
			// モデルの情報をそれぞれに合わせる
			MV1SetPosition(obj.modelHandle, obj.pos);
			MV1SetRotationXYZ(obj.modelHandle, obj.rot);
			MV1SetScale(obj.modelHandle, obj.scale);

			// 行列が変わるためコリジョン情報を更新
			MV1RefreshCollInfo(obj.modelHandle);

			// 線分とモデルの当たり判定
			MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(
				obj.modelHandle,
				obj.collisionFrame,// コリジョンフレーム指定
				lineStart,
				lineEnd
			);

			if (hitPoly.HitFlag) {
				// 当たった
				// 最も高いY位置を記録
				if (hitPoly.HitPosition.y > highestY) {
					highestY = hitPoly.HitPosition.y;
					hitAnyObj = true;
				}
			}
		}

		if (hitAnyObj) {
			// 地面が見つかった
			// 当たったY位置をキャラ座標にする
			testPos.y = highestY;

			// キャラの座標を更新
			_player->SetPos(testPos);
			isLanded = true;
			break;// ループから抜ける
		}
	}

	if (!isLanded) {
		// 地面が見つからなかった
		// 元の座標に戻す
		_player->SetPos(vOldPos);
	}
}

