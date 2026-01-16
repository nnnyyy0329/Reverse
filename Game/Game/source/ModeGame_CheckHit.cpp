#include "ModeGame.h"
#include "CharaBase.h"
#include "StageBase.h"
#include "AttackBase.h"

// キャラとマップの当たり判定
void ModeGame::CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara) 
{
	if(!chara || !_stage) return;

	auto& mapObjList = _stage->GetMapModelPosList();
	VECTOR vCurrentPos = chara->GetPos();// 現在の移動後座標
	VECTOR vOldPos = chara->GetOldPos();// 移動前の座標

	// 移動ベクトルと移動量を計算
	VECTOR vMove = VSub(vCurrentPos, vOldPos);
	auto moveLength = VSize(vMove);

	// 移動していないなら処理しない
	if(moveLength < 0.001f) return;

	// 移動角度を計算
	auto moveRad = atan2(vMove.z, vMove.x);

	// escapeTbl[]順に角度を変えて回避を試みる
	const float escapeTbl[] = {
		0, -10, 10, -20, 20, -30, 30, -40, 40, -50, 50, -60, 60, -70, 70, -80, 80,
	};

	bool isLanded = false;// 地面が見つかったか

	for(int i = 0; i < static_cast<int>(sizeof(escapeTbl) / sizeof(escapeTbl[0])); i++) {
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
		for(const auto& obj : mapObjList) {
			if(obj.collisionFrame == -1) continue;

			// 線分とモデルの当たり判定
			MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(
				obj.modelHandle, obj.collisionFrame, lineStart, lineEnd
			);

			if(hitPoly.HitFlag) {
				// 当たった
				// 最も高いY位置を記録
				if(hitPoly.HitPosition.y > highestY) {
					highestY = hitPoly.HitPosition.y;
					hitAnyObj = true;
				}
			}
		}

		if(hitAnyObj) {
			// 地面が見つかった
			// 当たったY位置をキャラ座標にする
			testPos.y = highestY;
			chara->SetPos(testPos);// キャラの座標を更新
			isLanded = true;
			break;// ループから抜ける
		}
	}

	if(!isLanded) {
		// 地面が見つからなかった
		// 元の座標に戻す
		chara->SetPos(vOldPos);
	}
}

// プレイヤーと敵の当たり判定
void ModeGame::CheckHitPlayerEnemy(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2)
{
	if(chara1 == nullptr || chara2 == nullptr) { return; }

	if(HitCheck_Capsule_Capsule(
		chara1->GetCollisionTop(), chara1->GetCollisionBottom(), chara1->GetCollisionR(),
		chara2->GetCollisionTop(), chara2->GetCollisionBottom(), chara2->GetCollisionR()
	) != false)
	{
		printfDx("Player and Enemy Hit!\n");
	}
}

// キャラと攻撃コリジョンの当たり判定
void ModeGame::CheckHitCharaAttackCol(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack)
{
	if(chara == nullptr || attack == nullptr) { return; }

	// 攻撃コリジョン情報を取得
	const ATTACK_COLLISION& col = attack->GetAttackCollision();

	// 当たり判定
	if(HitCheck_Capsule_Capsule
	(
		chara->GetCollisionTop(), chara->GetCollisionBottom(), chara->GetCollisionR(),
		col.attackColTop, col.attackColBottom, col.attackColR
	) != false)
	{
		printfDx("Chara and Attack Collision Hit!\n");
	}
}