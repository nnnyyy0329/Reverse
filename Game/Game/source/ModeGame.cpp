#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "ModeMenu.h"

#include "CharaBase.h"
#include "StageBase.h"
#include "Enemy.h"
#include "CameraManager.h"

// いったんこれ
#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"
#include "DebugCamera.h"

bool ModeGame::Initialize() 
{
	if (!base::Initialize()) { return false; }

	// PlayerManagerの初期化
	_playerManager = std::make_shared<PlayerManager>();
	_playerManager->Initialize();

	// プレイヤーの作成と登録
	{
		auto surfacePlayer = std::make_shared<SurfacePlayer>();
		surfacePlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::SURFACE, surfacePlayer);

		auto interiorPlayer = std::make_shared<InteriorPlayer>();
		interiorPlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::INTERIOR, interiorPlayer);
	}

	// いったんこれ
	_stage = std::make_shared<StageBase>(1);
	_cameraManager = std::make_shared<CameraManager>();
	_cameraManager->SetTarget(_playerManager->GetPlayerByType(PLAYER_TYPE::SURFACE));

	_debugCamera = std::make_shared<DebugCamera>();

	// 敵にターゲットのプレイヤーを設定
	for (const auto& enemy : _stage->GetEnemies()) {
		enemy->SetTarget(_playerManager->GetActivePlayerShared());
	}

	return true;
}

bool ModeGame::Terminate() 
{
	base::Terminate();


	// いったんこれ
	_playerManager.reset();


	return true;
}

bool ModeGame::Process()
{
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

		// プレイヤーマネージャーに入力状態を渡す
		if (_playerManager)
		{
			_playerManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if (_cameraManager)
		{
			_cameraManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
	}

	// spaceキーでメニューを開く
	if (ApplicationMain::GetInstance()->GetTrg() & PAD_INPUT_10) {
		ModeMenu* modeMenu = new ModeMenu();
		modeMenu->SetDebugCamera(_debugCamera);// デバッグカメラを渡す
		// ModeGameより上のレイヤーにメニューを登録する
		ModeServer::GetInstance()->Add(modeMenu, 99, "menu");
	}

	// オブジェクトの更新
	{
		_playerManager->Process();
		_stage->Process();
	}

	// 当たり判定
	{
		//CheckCollisionCharaMap(_player);
		for (const auto& enemy : _stage->GetEnemies()) {
			//CheckCollisionCharaMap(enemy);
		}
	}

	// いったん
	std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
	_cameraManager->SetTarget(activePlayer);	// 毎フレームプレイヤーにカメラを合わせる
	// 敵にターゲットのプレイヤーを設定
	for (const auto& enemy : _stage->GetEnemies()) {
		enemy->SetTarget(activePlayer);
	}

	_cameraManager->Process();

	return true;
}

bool ModeGame::Render() 
{
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	{
		SetUseLighting(TRUE);

		#if 0	// 平行ライト
			SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
			ChangeLightTypeDir(VGet(-1, -1, 0));
		#endif
		#if 1	// ポイントライト
			PlayerBase* activePlayer = _playerManager->GetActivePlayer();

			SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
			ChangeLightTypePoint(VAdd(activePlayer->GetPos(), VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
		#endif
	}

	// カメラ設定
	{
		// メニューが開いていて、デバッグカメラが使われているなら
		auto* menu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
		if (menu && menu->IsUseDebugCamera() && _debugCamera) {
			_debugCamera->SetUp();// デバッグカメラ設定更新
		}
		else {
			_cameraManager->SetUp();// カメラ設定更新
		}
	}
	
	// オブジェクトの描画
	{
		_playerManager->Render();
		_stage->Render();
	}

	// デバッグ情報の描画
	{
		//_player->DebugRender();
		_stage->DebugRender();
		_debugCamera->DebugRender();
	}

	return true;
}

// キャラとマップの当たり判定
void ModeGame::CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara) {
	if (!chara || !_stage) return;

	auto& mapObjList = _stage->GetMapModelPosList();
	VECTOR vCurrentPos = chara->GetPos();// 現在の移動後座標
	VECTOR vOldPos = chara->GetOldPos();// 移動前の座標

	// 移動ベクトルと移動量を計算
	VECTOR vMove = VSub(vCurrentPos, vOldPos);
	auto moveLength = VSize(vMove);

	// 移動していないなら処理しない
	if (moveLength < 0.001f) return;

	// 移動角度を計算
	auto moveRad = atan2(vMove.z, vMove.x);

	// escapeTbl[]順に角度を変えて回避を試みる
	const float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};

	bool isLanded = false;// 地面が見つかったか

	for (int i = 0; i < static_cast<int>(sizeof(escapeTbl) / sizeof(escapeTbl[0])); i++) {
		// escapeTbl[]分だけ角度をずらす
		float escapeRad = DEG2RAD(escapeTbl[i]);
		float checkRad = moveRad + escapeRad;

		// 判定用の移動ベクトル
		VECTOR testPos = vOldPos;
		testPos.x += cos(checkRad) * moveLength;
		testPos.z += sin(checkRad) * moveLength;

		// 判定用の線分の始点と終点を設定
		VECTOR lineStart = VAdd(testPos, VGet(0.0f, chara->GetColSubY(), 0.0f));// 始点：キャラの腰位置
		VECTOR lineEnd = VAdd(testPos, VGet(0.0f, -50.0f, 0.0f));

		// 当たった中で最も高い地面を記録する
		auto highestY = -99999.0f;
		bool hitAnyObj = false;

		// 全てのマップオブジェクトと当たり判定を行う
		for (const auto& obj : mapObjList) {
			if (obj.collisionFrame == -1) continue;

			// 線分とモデルの当たり判定
			MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(
				obj.modelHandle, obj.collisionFrame, lineStart, lineEnd
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
			chara->SetPos(testPos);// キャラの座標を更新
			isLanded = true;
			break;// ループから抜ける
		}
	}

	if (!isLanded) {
		// 地面が見つからなかった
		// 元の座標に戻す
		chara->SetPos(vOldPos);
	}
}
