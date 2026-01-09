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

// キャラとマップの当たり判定（接地維持 + カプセル押し出し方式）
// 前提：GetCollisionBottom()/Top() は「ワールド座標」をキャラ側で毎フレーム更新している
void ModeGame::CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara) {
	if (!chara || !_stage) return;

	const auto& mapObjList = _stage->GetMapModelPosList();

	// 現在の（移動入力反映後の）座標
	VECTOR vCurrentPos = chara->GetPos();

	// カプセル（半径のみキャラ設定から取得、線分端点はワールド座標をそのまま使う）
	const float capsuleRadius = chara->GetCollisionR();

	// ここで取得する Bottom/Top は「ワールド座標」前提
	VECTOR vCapBottomWS = chara->GetCollisionBottom();
	VECTOR vCapTopWS = chara->GetCollisionTop();

	// ---------------------------------------------------------
	// 1) 接地維持（床スナップ）
	// ---------------------------------------------------------
	// 足元（Bottom）から下に線分を飛ばして床を拾い、最も高い床へ吸着させる
	{
		const float groundProbeLen = 2000.0f; // ステージ規模に合わせて調整
		const float groundSnapEps = 1.0f;     // めり込み防止で少し浮かせる
		bool snapped = false;
		float bestHitY = -FLT_MAX;

		for (const auto& obj : mapObjList) {
			if (obj.collisionFrame == -1) continue;

			const VECTOR start = vCapBottomWS;
			const VECTOR end = VSub(vCapBottomWS, VGet(0.0f, groundProbeLen, 0.0f));

			auto hitLine = MV1CollCheck_Line(
				obj.modelHandle,
				obj.collisionFrame,
				start,
				end
			);

			if (!hitLine.HitFlag) continue;

			// 一番高い床を採用（段差で下の床に吸われにくくする）
			if (!snapped || hitLine.HitPosition.y > bestHitY) {
				bestHitY = hitLine.HitPosition.y;
				snapped = true;
			}
		}

		if (snapped) {
			const float targetBottomY = bestHitY + groundSnapEps;
			const float dy = targetBottomY - vCapBottomWS.y;

			// 位置とカプセル線分（WS）を同じだけ上げる
			vCurrentPos.y += dy;
			vCapBottomWS.y += dy;
			vCapTopWS.y += dy;
		}
	}

	// ---------------------------------------------------------
	// 2) カプセル押し出し（Iterative Solver）
	// ---------------------------------------------------------
	const float skin = 0.01f;
	const int maxSolveIters = 20;
	const float maxPushPerIter = capsuleRadius * 1.5f;

	for (int iter = 0; iter < maxSolveIters; ++iter) {
		bool anyHit = false;
		float bestPenetration = 0.0f;
		VECTOR vBestPush = VGet(0.0f, 0.0f, 0.0f);

		for (const auto& obj : mapObjList) {
			if (obj.collisionFrame == -1) continue;

			MV1_COLL_RESULT_POLY_DIM hit = MV1CollCheck_Capsule(
				obj.modelHandle,
				obj.collisionFrame,
				vCapBottomWS,
				vCapTopWS,
				capsuleRadius + skin
			);

			if (hit.HitNum > 0) {
				anyHit = true;

				for (int i = 0; i < hit.HitNum; ++i) {
					const VECTOR vP0 = hit.Dim[i].Position[0];
					const VECTOR vP1 = hit.Dim[i].Position[1];
					const VECTOR vP2 = hit.Dim[i].Position[2];

					const SegmentTriangleResult r = SegmentTriangleMinDistance(vCapBottomWS, vCapTopWS, vP0, vP1, vP2);

					const float dist = (r.fSegTriMinDistSquare > 0.0f) ? std::sqrt(r.fSegTriMinDistSquare) : 0.0f;
					const float penetration = (capsuleRadius + skin) - dist;
					if (penetration <= 1e-6f) continue;

					VECTOR vDir = VSub(r.vSegMinDistPos, r.vTriMinDistPos);
					if (VSize(vDir) < 1e-6f) {
						vDir = hit.Dim[i].Normal;
					}
					vDir = SafeNormalizeVec(vDir);

					const VECTOR vPush = VScale(vDir, penetration);

					if (penetration > bestPenetration) {
						bestPenetration = penetration;
						vBestPush = vPush;
					}
				}
			}

			MV1CollResultPolyDimTerminate(hit);
		}

		if (!anyHit || bestPenetration <= 0.0f) {
			break;
		}

		// 暴れ防止
		const float pushLen = VSize(vBestPush);
		if (pushLen > maxPushPerIter) {
			vBestPush = VScale(SafeNormalizeVec(vBestPush), maxPushPerIter);
		}

		// 位置 & カプセル線分（WS）を同じだけ移動
		vCurrentPos = VAdd(vCurrentPos, vBestPush);
		vCapBottomWS = VAdd(vCapBottomWS, vBestPush);
		vCapTopWS = VAdd(vCapTopWS, vBestPush);
	}

	// 最終反映
	chara->SetPos(vCurrentPos);
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
