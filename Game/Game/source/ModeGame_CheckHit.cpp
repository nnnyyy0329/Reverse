#include "ModeGame.h"
#include "CharaBase.h"
#include "StageBase.h"
#include "AttackBase.h"
#include "AttackManager.h"
#include "Bullet.h"
#include "BulletManager.h"
#include "DodgeSystem.h"
#include "GameCamera.h"
#include "SurfacePlayer.h"
#include "PlayerAbsorbAttackSystem.h" 
#include "AbsorbAttack.h"
#include "ModeTextBox.h"

// プレベータようパラメータ
namespace
{
	constexpr float consumeEnergy = 5.0f;
	constexpr float playerBulletDamage = 50.0f;
}

// キャラとマップの当たり判定と押し出し
void ModeGame::CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara)
{
	if (!chara || !_stage) { return; }

	// ステージの全マップモデルを取得
	const auto& mapObjList = _stage->GetMapModelPosList();
	if (mapObjList.empty()) { return; }

	// 前フレームの位置から現在位置への移動ベクトルを計算
	// すり抜け防止のため、移動量をステップ分割
	VECTOR oldPos = chara->GetOldPos();
	VECTOR currentPos = chara->GetPos();
	VECTOR totalMove = VSub(currentPos, oldPos);
	float moveLength = VSize(totalMove);

	// 移動量がほぼ0なら判定処理自体をスキップ
	const float minMoveThreshold = 0.001f;
	if (moveLength < minMoveThreshold) { return; }

	VECTOR moveDir = VNorm(totalMove);

	// キャラのカプセルを取得
	float capsuleRadius = chara->GetCollisionR();
	float capsuleHeight = chara->GetCollisionHeight();

	// ステップ移動の幅設定(カプセル半径の30%ごとに区切って移動)
	const float stepLength = capsuleRadius * 0.3f;

	// 判定に使う足元の座標
	VECTOR processPos = oldPos;

	// カプセル位置を足元基準で計算
	// Bottom = 足元 + 半径（カプセルの下端の球の中心）
	// Top = 足元 + (高さ - 半径)（カプセルの上端の球の中心）
	VECTOR capsuleBottom = VAdd(oldPos, VGet(0.0f, capsuleRadius, 0.0f));
	VECTOR capsuleTop = VAdd(oldPos, VGet(0.0f, capsuleHeight - capsuleRadius, 0.0f));

	// 接地情報の初期化
	bool isGrounded = false;
	const float initGroundY = -9999.0f;
	float highestGroundY = initGroundY;// 最も高い床のY座標

	// ループ管理用変数
	float movedDist = 0.0f;// 今回のフレームで移動した距離
	int stepCnt = 0;// ステップカウンタ(無限ループ防止)
	const int maxSteps = 512;// 最大ステップ分割数

	// 毎ステップごとのポリゴン検索範囲
	const float detectionMargin = 100.0f;// 少し広めに
	const float detectionRadius = capsuleRadius + detectionMargin;// 半径に加算する

	// メインループ : 移動距離分を少しづつ加算して判定
	while (movedDist < moveLength && stepCnt < maxSteps)
	{
		// 今回のステップで進む距離を決定
		float remainingDist = moveLength - movedDist;
		if (remainingDist <= 0.0f) { break; }
		float currentStepDist = (remainingDist < stepLength) ? remainingDist : stepLength;

		// 座標、カプセルを1ステップ分移動させる
		VECTOR vStepMove = VScale(moveDir, currentStepDist);
		processPos = VAdd(processPos, vStepMove);
		capsuleTop = VAdd(capsuleTop, vStepMove);
		capsuleBottom = VAdd(capsuleBottom, vStepMove);

		// 1.周囲のポリゴンを取得して壁と床に分類
		std::vector<MV1_COLL_RESULT_POLY> wallPolygons;// 壁ポリゴンリスト
		std::vector<MV1_COLL_RESULT_POLY> floorPolygons;// 床ポリゴンリスト

		// 検出球の中心をカプセルの中心に設定
		VECTOR detectionCenter = VAdd(processPos, VGet(0.0f, capsuleHeight * 0.5f, 0.0f));

		for (const auto& obj : mapObjList)
		{
			// コリジョンフレームがない、またはハンドルが無効
			if (obj.collisionFrame == -1 || obj.modelHandle <= 0) { continue; }

			// キャラ周辺のポリゴンを検出
			MV1_COLL_RESULT_POLY_DIM polyResult = MV1CollCheck_Sphere(
				obj.modelHandle,
				obj.collisionFrame,
				detectionCenter,
				detectionRadius
			);

			const float wallThreshold = 0.3f;// 壁判定の閾値
			for (int i = 0; i < polyResult.HitNum; ++i)
			{
				const MV1_COLL_RESULT_POLY& poly = polyResult.Dim[i];

				// 法線のY成分で壁と床を判定
				// Y成分がwallThreshold以下なら壁、それ以上なら床
				if (poly.Normal.y < wallThreshold)
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

		// 2:壁との衝突処理(押し出し)
		if (!wallPolygons.empty())
		{
			// 角で複数の壁と接触している場合を考慮して、ループで何度も押し出し処理を行う
			const int maxResolveLoop = 32;
			for (int loop = 0; loop < maxResolveLoop; ++loop)
			{
				bool allSeparated = true;// 全ての壁から抜け出せたか

				for (const auto& wall : wallPolygons)
				{
					// カプセルと三角形ポリゴンの当たり判定
					// 直前のポリゴンで押し出された最新の座標で判定
					if (!HitCheck_Capsule_Triangle(
						capsuleTop, capsuleBottom, capsuleRadius,
						wall.Position[0], wall.Position[1], wall.Position[2]))
					{
						continue;
					}

					allSeparated = false;// まだ壁に当たっている

					// 壁の法線からXZ平面の向きを取得
					VECTOR wallNormXZ = VGet(wall.Normal.x, 0.0f, wall.Normal.z);
					float normLen = VSize(wallNormXZ);
					if (normLen < 0.0001f) { continue; }
					wallNormXZ = VScale(wallNormXZ, 1.0f / normLen);// 正規化

					// コリジョンフレームの法線が内向きのため、逆方向へ押し出す
					// 小さい距離で何度も押し出し
					const float pushDist = 1.0f;
					VECTOR push = VScale(wallNormXZ, -pushDist);

					// 計算した押し出しベクトルを即座に反映
					processPos = VAdd(processPos, push);
					capsuleTop = VAdd(capsuleTop, push);
					capsuleBottom = VAdd(capsuleBottom, push);
				}

				// 全ての壁から離れたらループ終了
				if (allSeparated){ break; }
			}
		}

		// 3:床との接地判定
		if (!floorPolygons.empty())
		{
			// 足元の球の中心から線分を真下に伸ばす
			const float checkRange = capsuleRadius * 2.0f;
			VECTOR lineStart = VAdd(capsuleBottom, VGet(0.0f, -checkRange, 0.0f));
			VECTOR lineEnd = capsuleBottom;

			for (const auto& floor : floorPolygons)
			{
				// 線分と三角形ポリゴンの当たり判定
				HITRESULT_LINE hitResult = HitCheck_Line_Triangle(
					lineStart, lineEnd,
					floor.Position[0], floor.Position[1], floor.Position[2]);

				if (hitResult.HitFlag == 0) { continue; }

				// 交差点のY座標を取得
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
		movedDist += currentStepDist;
		++stepCnt;
	}

	// 設置処理の反映と最終座標の更新
	if (isGrounded)
	{
		// キャラのY座標を床の高さに合わせる
		processPos.y = highestGroundY;
		chara->SetIsStanding(true);
	}

	// 座標のみを反映(カプセル位置は各クラスで更新)
	chara->SetPos(processPos);
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
	VECTOR vDirNorm = mymath::FlattenVector(VSub(vChara2Center, vChara1Center));

	// 方向ベクトルの長さを計算(Y除去前の元ベクトルから)
	VECTOR vDirRaw = VSub(vChara2Center, vChara1Center);
	vDirRaw.y = 0.0f;
	float fDist = VSize(vDirRaw);

	// 距離がほぼ0の場合はラムダムな方向に押し出す
	const float constMinDistance = 0.001f;
	if (fDist < constMinDistance)
	{
		// ランダムな角度を生成
		float fRandAngle = mymath::RandomRange(0.0f, DX_TWO_PI_F);
		vDirNorm = VGet(sinf(fRandAngle), 0.0f, cosf(fRandAngle));
		fDist = 1.0f;
	}

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

	VECTOR vDir = VNorm(vToCam);

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

	auto bulletManager = BulletManager::GetInstance();
	if(bulletManager == nullptr){ return; }

	auto bullets = bulletManager->GetAllBullets();	// 登録された弾の取得

	std::vector<std::shared_ptr<Bullet>> deadBullets;// 削除する弾を一時保存するリスト

	// 全弾ループ
	for(auto& bullet : bullets)
	{
		if(!bullet) { continue; }

		// 登録されていない弾をスキップ
		if(!bulletManager->IsBulletRegistered(bullet)){ continue; }

		// 弾の発射者タイプを取得
		CHARA_TYPE bulletShooterType = bullet->GetShooterType();

		// 弾の設定情報を取得
		const BulletConfig& bulletConfig = bullet->GetBulletConfig();

		// 当たり判定
		if(HitCheck_Capsule_Sphere(
			chara->GetCollisionTop(), chara->GetCollisionBottom(), chara->GetCollisionR(),
			bullet->GetPos(), bulletConfig.radius
		))
		{
			// 同一所有者の弾かチェック
			if(IsSameOwnerBullet(myType, bulletShooterType))
			{
				continue; // 同じ所有者の弾はスキップ
			}

			// 当たった

			// 弾の設定からダメージを取得
			float damage = bulletConfig.damage;
			chara->ApplyDamageByBullet(damage, bullet->GetShooterType());

			deadBullets.push_back(bullet);// 削除リストに追加
		}
	}

	// 全ての判定が終わった後に、まとめて削除する
	for(const auto& deadBullet : deadBullets)
	{
		bulletManager->RemoveBullet(deadBullet);
	}
}

// 弾とマップの当たり判定
void ModeGame::CheckHitBulletMap()
{
	auto bulletManager = BulletManager::GetInstance();
	if (bulletManager == nullptr) { return; }

	const auto& mapObjList = _stage->GetMapModelPosList();
	if (mapObjList.empty()) { return; }

	auto bullets = bulletManager->GetAllBullets();

	std::vector<std::shared_ptr<Bullet>> deadBullets;

	for (auto& bullet : bullets)
	{
		if (!bullet) { continue; }

		if (!bulletManager->IsBulletRegistered(bullet)) { continue; }

		const BulletConfig& bulletConfig = bullet->GetBulletConfig();

		for (auto& obj : mapObjList)
		{
			if (obj.collisionFrame == -1 || obj.modelHandle <= 0) { continue; }

			MV1_COLL_RESULT_POLY_DIM result = MV1CollCheck_Sphere(
				obj.modelHandle,
				obj.collisionFrame,
				bullet->GetPos(),
				bulletConfig.radius
			);

			const float wallThreshold = 0.3f;
			bool hitWall = false;

			for(int i = 0; i < result.HitNum; ++i)
			{
				const MV1_COLL_RESULT_POLY& poly = result.Dim[i];

				if (poly.Normal.y < wallThreshold)
				{
					hitWall = true;
					break;
				}
			}

			MV1CollResultPolyDimTerminate(result);

			if (hitWall)
			{
				deadBullets.push_back(bullet);
				break;
			}
		}
	}

	for (const auto& deadBUllets : deadBullets)
	{
		bulletManager->RemoveBullet(deadBUllets);
	}
}

// 同一所有者の弾かどうかを判定
bool ModeGame::IsSameOwnerBullet(CHARA_TYPE targetType, CHARA_TYPE bulletShooterType)
{
	// プレイヤー系キャラの場合
	if(IsPlayerCharacter(targetType) && IsPlayerCharacter(bulletShooterType))
	{
		return true; // プレイヤー同士の弾は当たらない
	}

	// 敵キャラの場合
	if(targetType == CHARA_TYPE::ENEMY && bulletShooterType == CHARA_TYPE::ENEMY)
	{
		return true; // 敵同士の弾は当たらない
	}

	// 完全に同じタイプの場合
	if(targetType == bulletShooterType)
	{
		return true;
	}

	return false; // 異なる所有者
}

// プレイヤー系キャラかどうかを判定
bool ModeGame::IsPlayerCharacter(CHARA_TYPE charaType)
{
	return (charaType == CHARA_TYPE::SURFACE_PLAYER ||
			charaType == CHARA_TYPE::INTERIOR_PLAYER ||
			charaType == CHARA_TYPE::BULLET_PLAYER);
}

// キャラと攻撃コリジョンの当たり判定
void ModeGame::CheckActiveAttack(std::shared_ptr<CharaBase> chara)
{
	if(chara == nullptr) { return; }

	// AttackManagerから全てのアクティブな攻撃を取得
	auto activeAttacks = _attackManager->GetAllActiveAttacks();

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
	const AttackCollision& col = attack->GetAttackCollision();

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

// エネルギーによる変換処理
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
		_energyManager->ConvertDamageToConsumeEnergy(consumeEnergy);
	}
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
			if(hasHit)
			{
				int nextStageNum = _stage->GetNextStageNumFromTrigger(trigger.name);

				auto* self = this;
				auto modeTextBox = new ModeTextBox("GameSerif1", [self, nextStageNum]()
					{
						self->RequestStageChange(nextStageNum);
					});

				ModeServer::GetInstance()->Add(modeTextBox, 200, "textbox");
				return;
			}
		}
		else
		{
			// ポリゴン情報のメモリ解放
			MV1CollResultPolyDimTerminate(polyResult);
		}
	}
}

// 吸収攻撃の当たり判定チェック関数
void ModeGame::CheckHitAbsorbAttack(std::shared_ptr<CharaBase> player, std::shared_ptr<CharaBase>enemy)
{
	if(!player || !enemy){ return; }
	
	// SurfacePlayerかチェック
	auto surfacePlayer = std::dynamic_pointer_cast<SurfacePlayer>(player);
	if(!surfacePlayer){ return; }

	// 吸収攻撃システムを取得
	PlayerAbsorbAttackSystem* absorbSystemConst = surfacePlayer->GetAbsorbAttackSystem();
	if(!absorbSystemConst) { return; }

	// 吸収攻撃がアクティブかチェック
	if(!absorbSystemConst->IsAbsorbActive()){ return; }

	// 吸収の所有者を渡して判定
	CheckHitCharaAbsorbAttack(enemy, surfacePlayer, absorbSystemConst);
}

// キャラと吸収攻撃の当たり判定
void ModeGame::CheckHitCharaAbsorbAttack(std::shared_ptr<CharaBase> chara, std::shared_ptr<CharaBase> owner, PlayerAbsorbAttackSystem* absorbSystem)
{
	if(!chara || !owner || !absorbSystem){ return; }

	// 吸収攻撃コリジョン情報を取得
	const AbsorbConfig& config = absorbSystem->GetAbsorbConfig();

	if(!config.isActive){ return; }	// 吸収攻撃が有効でない場合は当たらない

	// 扇形データを取得
	SectorData sectorData;

	sectorData.center		= owner->GetPos();
	sectorData.direction	= owner->GetDir();
	sectorData.range		= config.absorbRange;
	sectorData.angle		= config.absorbAngle;
	sectorData.heightOffset = 0.0f;

	// GeometryUtilityを使用して扇形内判定
	if(GeometryUtility::IsInSector(chara->GetPos(), sectorData))
	{
		if(!config.isActive){ return; }	// 吸収攻撃が有効でない場合は当たらない

		// プレイヤーの吸収システムの更新呼び出し
		absorbSystem->ProcessAbsorb();
	}
}

// 攻撃所有者が自分に攻撃しているかどうか
bool ModeGame::OwnerIsAbsorbingOwner(std::shared_ptr<CharaBase> owner)
{
	if(owner->GetCharaType() == CHARA_TYPE::SURFACE_PLAYER ||
		owner->GetCharaType() == CHARA_TYPE::INTERIOR_PLAYER ||
		owner->GetCharaType() == CHARA_TYPE::BULLET_PLAYER)
	{
		// プレイヤー同士の攻撃は当たらない
		return true;
	}

	// その他のキャラタイプは吸収攻撃の対象外とする
	return false;
}