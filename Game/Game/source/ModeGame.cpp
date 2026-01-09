#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

#include "CharaBase.h"
#include "StageBase.h"
#include "EnemyBase.h"
#include "CameraManager.h"
#include "SurfacePlayer.h"

#include "CollisionSegmentTriangle.h"

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
		CheckCollisionCharaMap(_player);
		for (const auto& enemy : _stage->GetEnemies()) {
			CheckCollisionCharaMap(enemy);
		}
	}

	// 敵の視界判定
	//CheckEnemiesVision();

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
	const float moveLength = VSize(vMove);

	// 移動していないなら処理しない
	if (moveLength < 0.001f) return;

	// 移動角度を計算
	const float moveRad = atan2(vMove.z, vMove.x);

	// escapeTbl[]順に角度を変えて回避を試みる
	const float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};

	// カプセルパラメータ（必要ならキャラ側の値に合わせて調整）
	const float capsuleRadius = 12.0f;     // キャラの太さ（要調整）
	const float capsuleHalfHeight = 24.0f; // 上下の半分（要調整）

	// 押し出しの余裕（微小めり込みや数値誤差対策）
	const float skin = 0.1f;

	// 反復回数（めり込み解消用）
	const int maxSolveIters = 12;

	// 1反復での最大押し出し（暴れ対策）
	const float maxPushPerIter = capsuleRadius + 4.0f;

	bool isResolved = false;

	for (int i = 0; i < static_cast<int>(sizeof(escapeTbl) / sizeof(escapeTbl[0])); i++) {
		const float escapeRad = DEG2RAD(escapeTbl[i]);
		const float checkRad = moveRad + escapeRad;

		// 判定用の移動ベクトル（XZのみ調整）
		VECTOR vTestPos = vOldPos;
		vTestPos.x += cos(checkRad) * moveLength;
		vTestPos.z += sin(checkRad) * moveLength;

		// カプセル中心線（腰位置基準）
		const float waistY = chara->GetColSubY();
		VECTOR vCapCenter = VAdd(vTestPos, VGet(0.0f, waistY, 0.0f));
		VECTOR vCapStart = VAdd(vCapCenter, VGet(0.0f, -capsuleHalfHeight, 0.0f));
		VECTOR vCapEnd = VAdd(vCapCenter, VGet(0.0f, +capsuleHalfHeight, 0.0f));

		// 反復でめり込み解消
		for (int iter = 0; iter < maxSolveIters; ++iter) {

			bool anyHit = false;

			// 今回反復で「最も深いめり込み」を採用（押し出しは1つにする）
			float bestPenetration = 0.0f;
			VECTOR vBestPush = VGet(0.0f, 0.0f, 0.0f);

			for (const auto& obj : mapObjList) {
				if (obj.collisionFrame == -1) continue;

				MV1_COLL_RESULT_POLY_DIM hit = MV1CollCheck_Capsule(
					obj.modelHandle,
					obj.collisionFrame,
					vCapStart,
					vCapEnd,
					capsuleRadius
				);

				if (hit.HitNum > 0) {
					anyHit = true;

					// 返ってきた当たりポリゴン全部について、線分-三角形の最短距離から押し出し量を計算
					for (int a = 0; a < hit.HitNum; ++a) {
						const VECTOR vP0 = hit.Dim[a].Position[0];
						const VECTOR vP1 = hit.Dim[a].Position[1];
						const VECTOR vP2 = hit.Dim[a].Position[2];

						const SegmentTriangleResult r = SegmentTriangleMinDistance(vCapStart, vCapEnd, vP0, vP1, vP2);

						// distance
						const float distSq = r.fSegTriMinDistSquare;
						const float dist = (distSq > 0.0f) ? std::sqrt(distSq) : 0.0f;

						// penetration = radius - distance
						const float penetration = (capsuleRadius + skin) - dist;
						if (penetration <= 0.0f) continue;

						// 押し出し方向は (線分側最近接点 - 三角形側最近接点)
						VECTOR vDir = VSub(r.vSegMinDistPos, r.vTriMinDistPos);
						const float dirLen = VSize(vDir);

						// 方向が取れない場合は法線を使う（退避）
						if (dirLen < 1e-6f) {
							vDir = hit.Dim[a].Normal;
						}
						vDir = SafeNormalizeVec(vDir);

						VECTOR vPush = VScale(vDir, penetration);

						// この反復では最も深いめり込みを採用
						if (penetration > bestPenetration) {
							bestPenetration = penetration;
							vBestPush = vPush;
						}
					}
				}

				MV1CollResultPolyDimTerminate(hit);
			}

			if (!anyHit) {
				// もう当たっていない＝解決
				break;
			}

			// 押し出し候補が取れない（数値的におかしい）場合は終了
			if (bestPenetration <= 0.0f || (vBestPush.x == 0.0f && vBestPush.y == 0.0f && vBestPush.z == 0.0f)) {
				break;
			}

			// 押し出し量の制限（暴れ対策）
			const float pushLen = VSize(vBestPush);
			if (pushLen > maxPushPerIter) {
				vBestPush = VScale(SafeNormalizeVec(vBestPush), maxPushPerIter);
			}

			// 位置とカプセルを更新
			vTestPos = VAdd(vTestPos, vBestPush);
			vCapCenter = VAdd(vCapCenter, vBestPush);
			vCapStart = VAdd(vCapStart, vBestPush);
			vCapEnd = VAdd(vCapEnd, vBestPush);
		}

		// 最終確認：まだ当たるなら失敗扱い、当たらないなら採用
		bool finalHit = false;
		for (const auto& obj : mapObjList) {
			if (obj.collisionFrame == -1) continue;

			MV1_COLL_RESULT_POLY_DIM hit = MV1CollCheck_Capsule(
				obj.modelHandle,
				obj.collisionFrame,
				vCapStart,
				vCapEnd,
				capsuleRadius
			);

			if (hit.HitNum > 0) {
				finalHit = true;
			}

			MV1CollResultPolyDimTerminate(hit);

			if (finalHit) break;
		}

		if (!finalHit) {
			chara->SetPos(vTestPos);
			isResolved = true;
			break;
		}
	}

	if (!isResolved) {
		// 解決できなかったら元に戻す
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
