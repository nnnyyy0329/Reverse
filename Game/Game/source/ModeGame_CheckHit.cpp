#include "ModeGame.h"
#include "CharaBase.h"
#include "StageBase.h"
#include "AttackBase.h"
#include "AttackManager.h"
#include "BulletManager.h"
#include "Bullet.h"
#include "DodgeSystem.h"
#include "GameCamera.h"

// キャラとマップの当たり判定
void ModeGame::CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara)
{
	if (!chara || !_stage) { return; }

	// ステージの全マップモデルを取得
	const auto& mapObjList = _stage->GetMapModelPosList();
	if (mapObjList.empty()) { return; }

	// 移動前後の座標を取得
	VECTOR vOldPos = chara->GetOldPos();
	VECTOR vCurrentPos = chara->GetPos();
	VECTOR vTotalMove = VSub(vCurrentPos, vOldPos);// 総移動ベクトル
	float moveLength = VSize(vTotalMove);// 移動距離

	// 移動量が微小な場合は処理をスキップ
	const float constMinMoveThreshold = 0.001f;
	if (moveLength < constMinMoveThreshold) { return; }

	VECTOR vMoveDir = VNorm(vTotalMove);// 移動方向の正規化ベクトル

	// キャラのカプセル半径と高さを取得
	float capsuleRadius = chara->GetCollisionR();
	//const float constDefaultRadius = 1.0f;
	//if (capsuleRadius <= 0.0f) { capsuleRadius = constDefaultRadius; }

	float capsuleHeight = chara->GetCollisionHeight();
	//const float constDefaultHeight = 100.0f;
	//if (capsuleHeight <= 0.0f) { capsuleHeight = constDefaultHeight; }

	// ステップ移動(すり抜け防止)
	// カプセル半径*2の距離ごとに移動を分割して判定を行う
	const float stepLength = capsuleRadius * 2.0f;

	// _vPosはキャラの足元座標
	VECTOR vProcessPos = vOldPos;

	// カプセル位置を足元基準で計算
	// Bottom = 足元 + 半径（カプセルの下端の球の中心）
	// Top = 足元 + (高さ - 半径)（カプセルの上端の球の中心）
	VECTOR vCapsuleBottom = VAdd(vOldPos, VGet(0.0f, capsuleRadius, 0.0f));
	VECTOR vCapsuleTop = VAdd(vOldPos, VGet(0.0f, capsuleHeight - capsuleRadius, 0.0f));

	// 接地情報の初期化
	bool isGrounded = false;
	const float constInitGroundY = -9999.0f;
	float highestGroundY = constInitGroundY;// 最も高い床のY座標

	// ステップ移動の進行状況を管理
	float movedDistance = 0.0f;// 移動済み距離
	int stepCnt = 0;// ステップカウンタ(無限ループ防止)
	const int constMaxSteps = 256;// 最大ステップ数

	// メインループ:移動をステップごとに処理
	while (movedDistance < moveLength && stepCnt < constMaxSteps)
	{
		// 残りの移動距離を計算
		float remainingDist = moveLength - movedDistance;
		if (remainingDist <= 0.0f) { break; }

		// 今回のステップで移動する距離を決定(残り距離と上限の小さいほう)
		float currentStepDist = (remainingDist < stepLength) ? remainingDist : stepLength;
		VECTOR vStepMove = VScale(vMoveDir, currentStepDist);

		// 座標とカプセル位置をステップ分だけ移動
		vProcessPos = VAdd(vProcessPos, vStepMove);
		vCapsuleTop = VAdd(vCapsuleTop, vStepMove);
		vCapsuleBottom = VAdd(vCapsuleBottom, vStepMove);

		// ステップ1:周囲のポリゴンを取得
		// 球範囲でポリゴンを取得して壁と床に分類
		std::vector<MV1_COLL_RESULT_POLY> wallPolygons;// 壁ポリゴンリスト
		std::vector<MV1_COLL_RESULT_POLY> floorPolygons;// 床ポリゴンリスト

		// 検出範囲の中心をカプセルの中心に設定
		const float constDetectionMargin = 50.0f;// 少し広めに
		const float detectionRadius = capsuleRadius + constDetectionMargin;// 半径に加算する
		VECTOR vDetectionCenter = VAdd(vProcessPos, VGet(0.0f, capsuleHeight * 0.5f, 0.0f));

		// 全マップモデルを走査
		for (const auto& obj : mapObjList)
		{
			// コリジョンフレームがない、またはハンドルが無効
			if (obj.collisionFrame == -1 || obj.modelHandle <= 0) { continue; }

			// 球範囲内にあるモデルのポリゴンを取得
			MV1_COLL_RESULT_POLY_DIM polyResult = MV1CollCheck_Sphere(
				obj.modelHandle,
				obj.collisionFrame,
				vDetectionCenter,
				detectionRadius
			);

			// 取得したポリゴンを壁と床に分類
			const float constWallThreshold = 0.2f;// 壁判定の閾値
			for (int i = 0; i < polyResult.HitNum; ++i)
			{
				const MV1_COLL_RESULT_POLY& poly = polyResult.Dim[i];

				// 法線のY成分で壁と床を判定
				// Y成分が0.2以下なら壁、それ以上なら床
				if (poly.Normal.y < constWallThreshold)
				{
					wallPolygons.push_back(poly);
				}
				else
				{
					floorPolygons.push_back(poly);
				}
			}

			// ポリゴン情報のメモリ解放
			MV1CollResultPolyDimTerminate(polyResult);
		}

		// ステップ2:壁との衝突処理
		if (!wallPolygons.empty())
		{
			bool hasSlided = false;// スライド移動を適用したか

			// 押し出しループ
			// 最大16回までループしてすべての壁との衝突を解消する
			const int constMaxResolveLoop = 16;
			for (int loop = 0; loop < constMaxResolveLoop; ++loop)
			{
				bool allSeparated = true;// 全ての壁から離れたか
				VECTOR vTotalPush = VGet(0.0f, 0.0f, 0.0f);// 今回のループでの総押し出しベクトル

				// 全ての壁ポリゴンをチェック
				for (const auto& wall : wallPolygons)
				{
					// カプセルと三角形ポリゴンの当たり判定
					if (!HitCheck_Capsule_Triangle(
						vCapsuleTop, vCapsuleBottom, capsuleRadius,
						wall.Position[0], wall.Position[1], wall.Position[2]))
					{
						continue;// この壁とは衝突していない
					}

					allSeparated = false;// まだ壁と接触している

					// 壁の法線からXZ平面成分を抽出
					VECTOR vWallNormXZ = VGet(wall.Normal.x, 0.0f, wall.Normal.z);
					float normLen = VSize(vWallNormXZ);

					// 法線が無効な場合はスキップ
					const float constMinNormalLen = 0.0001f;
					if (normLen < constMinNormalLen) { continue; }

					// 正規化
					vWallNormXZ = VScale(vWallNormXZ, 1.0f / normLen);

					// 壁からの押し出し処理
					// 正規化された法線方向に少しずつ押し出す
					const float constPushDistance = 1.0f;
					VECTOR vPush = VScale(vWallNormXZ, constPushDistance);

					// 総押し出しベクトルに加算
					vTotalPush = VAdd(vTotalPush, vPush);
				}

				// 総押し出しベクトルを一度に適用
				if (VSize(vTotalPush) > 0.0001f)
				{
					vProcessPos = VAdd(vProcessPos, vTotalPush);
					vCapsuleTop = VAdd(vCapsuleTop, vTotalPush);
					vCapsuleBottom = VAdd(vCapsuleBottom, vTotalPush);
				}

				// 全ての壁から離れたらループ終了
				if (allSeparated)
				{
					break;
				}
			}

			// スライド移動処理を押し出し処理の後に実行
			bool bHasCollision = false;
			VECTOR vSlideMove = VGet(0.0f, 0.0f, 0.0f);

			//壁との衝突をチェック
			for (const auto& wall : wallPolygons)
			{
				// カプセルと三角形ポリゴンの当たり判定
				if (HitCheck_Capsule_Triangle(
					vCapsuleTop, vCapsuleBottom, capsuleRadius,
					wall.Position[0], wall.Position[1], wall.Position[2]))
				{
					bHasCollision = true;

					// 壁の法線からXZ平面成分を抽出
					VECTOR vWallNormXZ = VGet(wall.Normal.x, 0.0f, wall.Normal.z);
					float normLen = VSize(vWallNormXZ);

					// 法線が無効な場合はスキップ
					const float constMinNormalLen = 0.0001f;
					if (normLen < constMinNormalLen)
					{
						continue;
					}

					// 正規化
					vWallNormXZ = VScale(vWallNormXZ, 1.0f / normLen);

					// 移動ベクトルを壁に沿ってスライドさせる
					// 壁法線との内積を計算
					float dotProduct = VDot(vStepMove, vWallNormXZ);

					// 壁に向かっている場合のみスライド処理
					if (dotProduct < 0.0f)
					{
						// 壁方向の成分を除去してスライドベクトルを計算
						VECTOR vSlide = VSub(vStepMove, VScale(vWallNormXZ, dotProduct));
						vSlide.y = 0.0f; // 水平方向のみに制限

						// スライドベクトルが有効な場合のみ保存
						if (VSize(vSlide) > VSize(vSlideMove))
						{
							vSlideMove = vSlide;
						}
					}
				}
			}

			// スライド移動を適用
			const float constMinSlideThreshold = 0.0001f;
			if (bHasCollision && VSize(vSlideMove) > constMinSlideThreshold)
			{
				vProcessPos = VAdd(vProcessPos, vSlideMove);
				vCapsuleTop = VAdd(vCapsuleTop, vSlideMove);
				vCapsuleBottom = VAdd(vCapsuleBottom, vSlideMove);
			}
		}

		// ステップ3:床との接地判定
		if (!floorPolygons.empty())
		{
			// 足元から少し下の範囲をチェック
			const float constCheckRange = capsuleRadius * 2.0f;
			VECTOR vLineStart = VAdd(vCapsuleBottom, VGet(0.0f, -constCheckRange, 0.0f));// 足元から下方向へ
			VECTOR vLineEnd = vCapsuleBottom;// 足元の球の中心

			// 全ての床ポリゴンをチェック
			for (const auto& floor : floorPolygons)
			{
				// 線分と三角形ポリゴンの当たり判定
				HITRESULT_LINE hitResult = HitCheck_Line_Triangle(
					vLineStart, vLineEnd,
					floor.Position[0], floor.Position[1], floor.Position[2]);

				if (hitResult.HitFlag == 0)
				{
					continue;// この床とは交差していない
				}

				// 実際の交差点のY座標を使用
				float floorY = hitResult.Position.y;

				// 最も高い床のY座標を記録
				if (floorY > highestGroundY)
				{
					highestGroundY = floorY;
					isGrounded = true;
				}
			}
		}

		// 進行状況を更新
		movedDistance += currentStepDist;
		++stepCnt;
	}

	// 接地状態に応じた座標補正
	if (isGrounded)
	{
		// 現在の高さよりも高い床があればY座標を補正
		if (highestGroundY > vProcessPos.y)
		{
			vProcessPos.y = highestGroundY;
		}
	}

	// 座標のみを反映(カプセル位置は各クラスで更新)
	chara->SetPos(vProcessPos);
}

// キャラ同士の当たり判定
void ModeGame::CheckCollisionCharaChara(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2)
{
	if (!chara1 || !chara2) { return; }

	if (chara1 == chara2) { return; }

	VECTOR vChara1Top = chara1->GetCollisionTop();// キャラ1のカプセル上端
	VECTOR vChara1Bottom = chara1->GetCollisionBottom();// キャラ1のカプセル下端
	float fChara1Rad = chara1->GetCollisionR();// キャラ1のカプセル半径

	VECTOR vChara2Top = chara2->GetCollisionTop();// キャラ2のカプセル上端
	VECTOR vChara2Bottom = chara2->GetCollisionBottom();// キャラ2のカプセル下端
	float fChara2Rad = chara2->GetCollisionR();// キャラ2のカプセル半径

	// カプセル同士の当たり判定
	if (!HitCheck_Capsule_Capsule(
		vChara1Top, vChara1Bottom, fChara1Rad,
		vChara2Top, vChara2Bottom, fChara2Rad))
	{
		return;// 衝突していない
	}

	// 押し出し処理
	// 各キャラの中心位置を計算
	VECTOR vChara1Center = VAdd(vChara1Bottom, VScale(VSub(vChara1Top, vChara1Bottom), 0.5f));
	VECTOR vChara2Center = VAdd(vChara2Bottom, VScale(VSub(vChara2Top, vChara2Bottom), 0.5f));

	// キャラ1からキャラ2への方向ベクトルを計算(XZ平面のみ)
	VECTOR vDir = VSub(vChara2Center, vChara1Center);
	vDir.y = 0.0f; // Y軸は無視

	// 方向ベクトルの長さを計算
	float fDist = VSize(vDir);

	// 距離がほぼ0の場合はラムダムな方向に押し出す
	const float constMinDistance = 0.001f;
	if (fDist < constMinDistance)
	{
		// ランダムな角度を生成
		float fRandAngle = static_cast<float>(GetRand(359)) * DX_PI_F / 180.0f;
		vDir = VGet(sinf(fRandAngle), 0.0f, cosf(fRandAngle));
		fDist = 1.0f;// 正規化のために1に
	}

	// 方向ベクトルを正規化
	VECTOR vDirNorm = VScale(vDir, 1.0f / fDist);

	// 必要な押し出し距離を計算
	// 両方のカプセル半径の合計から現在の距離を引く
	float fRequiredDist = (fChara1Rad + fChara2Rad) - fDist;

	// 押し出しが必要ない場合は終了
	if (fRequiredDist <= 0.0f) { return; }

	// 各キャラを半分ずつ押し出す
	float fPushDist = fRequiredDist * 0.5f;

	// キャラ1を逆方向に押し出す
	VECTOR vPush1 = VScale(vDirNorm, -fPushDist);
	VECTOR vNewPos1 = VAdd(chara1->GetPos(), vPush1);
	chara1->SetPos(vNewPos1);

	// キャラ2を正方向に押し出す
	VECTOR vPush2 = VScale(vDirNorm, fPushDist);
	VECTOR vNewPos2 = VAdd(chara2->GetPos(), vPush2);
	chara2->SetPos(vNewPos2);
}

// カメラとマップの当たり判定
void ModeGame::CheckCollisionCameraMap()
{
	if (!_gameCamera || !_stage) { return; }

	// ステージの全マップモデルを取得
	const auto& mapObjList = _stage->GetMapModelPosList();
	if (mapObjList.empty()) { return; }

	// 元のカメラ情報を取得
	VECTOR vCamPos = _gameCamera->GetVPos();
	VECTOR vCamTarget = _gameCamera->GetVTarget();

	// 注視点からカメラへのベクトル
	VECTOR vToCam = VSub(vCamPos, vCamTarget);
	float fCamDist = VSize(vToCam);
	const float conMinCamDist = 0.001f;
	if (fCamDist < conMinCamDist) { return; }// 距離が近すぎる場合は処理しない

	VECTOR vDir = VScale(vToCam, 1.0f / fCamDist);

	// 球を使った当たり判定
	const float fCamRad = 10.0f;// 半径
	// 検出範囲の中心点と半径
	VECTOR vMid = VAdd(vCamTarget, VScale(vDir, fCamDist * 0.5f));
	const float fSphereMargin = 5.0f;
	float fDetectRad = fCamDist * 0.5f + fCamRad + fSphereMargin;

	bool bHasHit = false;
	float fNearestDist = fCamDist;// 最も近い衝突距離

	// 全マップモデルを走査
	for (const auto& obj : mapObjList)
	{
		// コリジョンフレームがない、またはハンドルが無効
		if (obj.collisionFrame == -1 || obj.modelHandle <= 0) { continue; }

		MV1_COLL_RESULT_POLY_DIM hitPolys = MV1CollCheck_Sphere(
			obj.modelHandle,
			obj.collisionFrame,
			vMid,
			fDetectRad
		);

		// プレイヤーとカメラを結ぶ線分の間にぶつかるポリゴンがるかチェック
		for (int i = 0; i < hitPolys.HitNum; ++i)
		{
			const MV1_COLL_RESULT_POLY& poly = hitPolys.Dim[i];

			// 注視点からカメラへの線分とポリゴンの当たり判定
			HITRESULT_LINE hitResult = HitCheck_Line_Triangle(
				vCamTarget,
				vCamPos,
				poly.Position[0],
				poly.Position[1],
				poly.Position[2]
			);

			if (hitResult.HitFlag == 0) { continue; }

			// 衝突点までの距離を計算
			float fHitDist = VSize(VSub(hitResult.Position, vCamTarget));
			if (fHitDist < fNearestDist)
			{
				fNearestDist = fHitDist;
				bHasHit = true;
			}
		}

		// ポリゴン情報のメモリ解放
		MV1CollResultPolyDimTerminate(hitPolys);
	}

	// 当たっていたら手前に補正
	if (bHasHit)
	{
		// 当たった位置からカメラ半径分だけ手前に移動
		float fNewDist = fNearestDist - fCamRad;
		if (fNewDist < 0.0f) { fNewDist = 0.0f; }

		// 注視点から新しいカメラ位置を計算
		vCamPos = VAdd(vCamTarget, VScale(vDir, fNewDist));
		_gameCamera->SetVPos(vCamPos);
	}
}

// キャラと弾の当たり判定
void ModeGame::CheckHitCharaBullet(std::shared_ptr<CharaBase> chara)
{
	if(!chara) { return; }

	CHARA_TYPE myType = chara->GetCharaType();// 自分のキャラタイプを取得

	const auto& bullets = _bulletManager->GetBullets();// 弾のリストを取得

	std::vector<std::shared_ptr<Bullet>> deadBullets;// 削除する弾を一時保存するリスト

	// 全弾ループ
	for(const auto& bullet : bullets)
	{
		if(!bullet) { continue; }

		// キャラと弾のタイプが同じなら無視する
		if(bullet->GetShooterType() == myType) { continue; }

		// 当たり判定
		if(HitCheck_Capsule_Sphere(
			chara->GetCollisionTop(), chara->GetCollisionBottom(), chara->GetCollisionR(),
			bullet->GetPos(), bullet->GetCollisionR()
		))
		{
			// 当たった

			// ダメージ処理とか
			float damage = 1000.0f;
			chara->ApplyDamageByBullet(damage, bullet->GetShooterType());
			deadBullets.push_back(bullet);// 削除リストに追加
		}
	}

	// 全ての判定が終わった後に、まとめて削除する
	for(const auto& deadBullet : deadBullets)
	{
		_bulletManager->RemoveBullet(deadBullet);
	}
}

// キャラと攻撃コリジョンの当たり判定
void ModeGame::CheckActiveAttack(std::shared_ptr<CharaBase> chara)
{
	if(chara == nullptr) { return; }

	// AttackManagerから全てのアクティブな攻撃を取得
	auto* attackManager = AttackManager::GetInstance();
	auto activeAttacks = attackManager->GetAllActiveAttacks();

	// 各攻撃と当たり判定
	for(auto& attack : activeAttacks)
	{
		if(attack == nullptr) continue;

		// 当たり判定
		CheckHitCharaAttackCol(chara, attack);
	}
}

// キャラと攻撃コリジョンの当たり判定
void ModeGame::CheckHitCharaAttackCol(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack)
{
	if(chara == nullptr || attack == nullptr) { return; }

	// 既にヒット済みのキャラかチェック
	if(attack->HasHitCharas(chara)) { return; }

	// 無敵状態かチェック
	bool isInvincible = false;
	if(_dodgeSystem && _dodgeSystem->IsCharacterInvincible(chara))
	{
		isInvincible = true;
	}

	// 回避済みの攻撃かチェック
	if(_attackManager->IsDodgeHitAttack(attack))
	{
		return;
	}

	// 攻撃コリジョン情報を取得
	const ATTACK_COLLISION& col = attack->GetAttackCollision();

	// 当たり判定
	if(HitCheck_Capsule_Capsule
	(
		chara->GetCollisionTop(), chara->GetCollisionBottom(), chara->GetCollisionR(),
		col.attackColTop, col.attackColBottom, col.attackColR
	) != false)
	{
		// ヒットフラグを有効にする
		attack->SetHitFlag(true);

		// 無敵状態の場合は回避ヒット扱いにして登録
		if(isInvincible)
		{
			_attackManager->RegisterDodgeHitAttack(attack);
			isInvincible = false;
			return;
		}

		// ヒットしたキャラを登録
		attack->AddHitCharas(chara);

		//EffectServer::GetInstance()->Play("SurfacePlayerAttackHit1", chara->GetPos());

		auto ownerType = _attackManager->GetAttackOwnerType(attack);	// 攻撃の所有者タイプ取得
		auto charaType = chara->GetCharaType();							// キャラのタイプ取得

		// 自分に攻撃しているかどうか
		if(OwnerIsAttackingOwner(charaType, ownerType)){ return; }
		
		// ダメージ処理
		float damage = attack->GetDamage();			// ダメージ取得
		auto beforeLife = chara->GetLife();			// ヒット前のライフ取得

		chara->ApplyDamage(damage, ownerType, col);	// ターゲットにダメージを与える

		auto afterLife = chara->GetLife();			// ヒット後のライフ取得
		damage = beforeLife - afterLife;			// 実際に与えたダメージを計算

		// ダメージをエネルギーに変換
		ConvertEnergy(attack, damage);									
	}
}

// ダメージをエネルギーに変換する
void ModeGame::ConvertEnergy(std::shared_ptr<AttackBase> attack, float damage)
{
	// 攻撃管理クラスから所有者情報を取得
	ATTACK_OWNER_TYPE ownerType = _attackManager->GetAttackOwnerType(attack);

	// 表プレイヤーならエネルギー獲得
	if(ownerType == ATTACK_OWNER_TYPE::SURFACE_PLAYER)
	{
		// 変換
		_energyManager->ConvertDamageToEnergy(damage);
	}

	// 裏プレイヤーならエネルギー消費
	else if(ownerType == ATTACK_OWNER_TYPE::INTERIOR_PLAYER)
	{
		// 消費変換
		_energyManager->ConvertDamageToConsumeEnergy(damage);
	}
}

// 攻撃所有者が自分に攻撃しているかどうか
bool ModeGame::OwnerIsAttackingOwner(CHARA_TYPE charaType, ATTACK_OWNER_TYPE ownerType)
{
	// 同じキャラからの攻撃かチェック
	switch(charaType)
	{
		case CHARA_TYPE::ENEMY:	// 敵キャラ
		{
			// 敵キャラの場合、攻撃所有者が敵であるかを確認
			return (ownerType == ATTACK_OWNER_TYPE::ENEMY);
		}

		case CHARA_TYPE::SURFACE_PLAYER:	// 表プレイヤー
		case CHARA_TYPE::INTERIOR_PLAYER:	// 裏プレイヤー
		{
			// プレイヤー同士の攻撃は当たらない
			return (ownerType == ATTACK_OWNER_TYPE::SURFACE_PLAYER ||
					ownerType == ATTACK_OWNER_TYPE::INTERIOR_PLAYER);
		}

		default:	// その他のキャラタイプ
		{
			return false;
		}
	}

	return false;
}

// プレイヤーとトリガーの当たり判定
void ModeGame::CheckHitPlayerTrigger(std::shared_ptr<CharaBase> player)
{
	if (!player || !_stage) { return; }

	const auto& triggerList = _stage->GetTriggerList();
	if (triggerList.empty()) { return; }

	// プレイヤーの現在位置を取得
	VECTOR vCurrentPos = player->GetPos();

	// プレイヤーのカプセル情報を取得
	float capsuleR = player->GetCollisionR();
	float capsuleH = player->GetCollisionHeight();

	// カプセル位置を足元基準で計算
	VECTOR vCapsuleBottom = player->GetCollisionBottom();
	VECTOR vCapsuleTop = player->GetCollisionTop();

	// カプセルの中心位置を計算
	VECTOR vCapsuleCenter = VAdd(vCapsuleBottom, VScale(VSub(vCapsuleTop, vCapsuleBottom), 0.5f));

	// 検出範囲の中心をカプセルの中心に設定
	const float constDetectionMargin = 50.0f;
	const float detectionRadius = capsuleR + constDetectionMargin;

	// 全てのトリガーを走査
	for (const auto& trigger : triggerList)
	{
		if (trigger.modelHandle <= 0 || trigger.collisionFrame == -1) { continue; }

		// 球範囲内にあるモデルのポリゴンを取得
		MV1_COLL_RESULT_POLY_DIM polyResult = MV1CollCheck_Sphere(
			trigger.modelHandle,
			trigger.collisionFrame,
			vCapsuleCenter,
			detectionRadius
		);

		// ポリゴンが検出された
		if (polyResult.HitNum > 0)
		{
			bool hasHit = false;

			// 全てのポリゴンとカプセルの当たり判定
			for (int i = 0; i < polyResult.HitNum; ++i)
			{
				const MV1_COLL_RESULT_POLY& poly = polyResult.Dim[i];

				// カプセルと三角形ポリゴンの当たり判定
				if (HitCheck_Capsule_Triangle(
					vCapsuleTop, vCapsuleBottom, capsuleR,
					poly.Position[0], poly.Position[1], poly.Position[2]))
				{
					hasHit = true;
					break; // 一つでもヒットしたループを抜ける
				}
			}

			// ポリゴン情報のメモリ解放
			MV1CollResultPolyDimTerminate(polyResult);

			// トリガーに当たった場合の処理
			if (hasHit)
			{
				// 次のステージ番号を取得
				int nextStageNum = _stage->GetNextStageNumFromTrigger(trigger.name);

				// ステージ切り替えリクエスト
				RequestStageChange(nextStageNum);

				_currentStageNum = nextStageNum;// 現在のステージ番号を更新

				return;// 一つのトリガーに当たったら処理を終了
			}
		}
		else
		{
			// ポリゴン情報のメモリ解放
			MV1CollResultPolyDimTerminate(polyResult);
		}
	}

}