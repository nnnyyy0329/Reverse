#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

#include "CharaBase.h"
#include "StageBase.h"
#include "EnemyBase.h"
#include "CameraManager.h"
#include "SurfacePlayer.h"

bool ModeGame::Initialize() 
{
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

bool ModeGame::Terminate() 
{
	base::Terminate();

	_player.reset();

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

		// プレイヤーに入力状態を渡す
		if (_player) 
		{
			_player->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if (_cameraManager)
		{
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
		//CheckCollisionCharaMap(_player);
		for (const auto& enemy : _stage->GetEnemies()) {
			CheckCollisionCharaMap(enemy);
		}
	}

	// 敵の視界判定
	CheckEnemiesVision();

	_cameraManager->Process();// カメラ更新

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

#if 1	// 平行ライト
		SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
		ChangeLightTypeDir(VGet(-1, -1, 0));
#endif
#if 0	// ポイントライト
		SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
		ChangeLightTypePoint(VAdd(_player->GetPos(), VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
#endif
	}

	_cameraManager->SetUp();// カメラ設定更新
	
	// オブジェクトの描画
	{
		_player->Render();
		_stage->Render();
	}

	// デバッグ情報の描画
	{
		//_player->DebugRender();
		_stage->DebugRender();
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

// 敵の視界判定
void ModeGame::CheckEnemiesVision() {
	auto enemies = _stage->GetEnemies();// ステージの敵リストを取得

	for (auto enemy : enemies) {
		if (!enemy) continue;

		// 距離チェック
		auto dist = VSize(VSub(_player->GetPos(), enemy->GetPos()));// プレイヤーと敵の距離
		if (dist > enemy->GetVisionRange()) continue;// 索敵距離外ならスキップ

		// 視界角度チェック
		VECTOR vToPlayer = VNorm(VSub(_player->GetPos(), enemy->GetPos()));// 敵からのプレイヤーへの単位ベクトル
		auto dot = VDot(enemy->GetDir(), vToPlayer);// 敵の向きベクトルとの内積を計算
		// 内積がcos以上なら視界角度内にいる
		if (dot < enemy->GetVisionCosAngle()) {
			continue;// 視界角度の外にいる
		}

		// 障害物チェック(実装予定)

		// ここまで来たらプレイヤーを発見している
		enemy->OnDetectPlayer(_player);// 追跡対象を設定
	}
}
