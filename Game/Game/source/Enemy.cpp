#include "Enemy.h"
#include "BulletManager.h"
#include "AttackBase.h"
#include "AttackManager.h"

namespace {
	constexpr auto COLLISION_RADIUS = 30.0f;// 敵の当たり判定半径
	constexpr auto COLLISION_HEIGHT = 100.0f;// 敵の当たり判定高さ
}

Enemy::Enemy(){
	// モデル読み込み
	_iHandle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 0, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	_vDir = VGet(0.0f, 0.0f, 1.0f);// 前方を向いておく

	// 腰位置の設定(マップモデルとの判定用)
	_colSubY = 40.0f;

	// 当たり判定用(カプセル)
	_fCollisionR = COLLISION_RADIUS;
	_fCollisionHeight = COLLISION_HEIGHT;
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionTop = VGet(0.0f, _fCollisionHeight, 0.0f);

	SetCharaType(CHARA_TYPE::ENEMY);// キャラタイプを設定
}

Enemy::~Enemy() {
}

bool Enemy::Initialize() {
	_vOldPos = _vPos;
	_vHomePos = _vPos;// 初期位置を保存

	return true;
}

bool Enemy::Terminate() {
	return true;
}

bool Enemy::Process() {
	_fPlayTime += 0.5f;// 再生時間を進める
	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if (_fPlayTime >= _fTotalTime) {
		_fPlayTime = 0.0f;
	}

	_vOldPos = _vPos;// 前フレームの位置を保存

	_vMove = VGet(0.0f, 0.0f, 0.0f);// 毎フレーム移動量リセット

	// 状態更新
	if (_currentState) {
		auto nextState = _currentState->Update(this);
		if (nextState) {
			ChangeState(nextState);
		}
	}

	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// カプセルに座標を対応させる
	_vCollisionBottom = VAdd(_vPos, VGet(0, _fCollisionR, 0));// 半径分ずらして中心位置に
	_vCollisionTop = VAdd(_vPos, VGet(0, _fCollisionHeight - _fCollisionR, 0));// 高さ分ずらす

	return true;
}

bool Enemy::Render() {
	// 再生時間をセットする
	MV1SetAttachAnimTime(_iHandle, _iAttachIndex, _fPlayTime);

	// モデルを描画する
	VECTOR vRot = {0, 0, 0};
	vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);// モデル標準でどちらを向いているかで式が変わる
	MATRIX mRotY = MGetRotY(vRot.y);// Y軸回転行列を作成

	// 位置移動行列を作成
	MATRIX mTrans = MGetTranslate(_vPos);

	// スケール行列を作成
	MATRIX mScale = MGetScale(_vScale);

	// 回転 -> 移動 -> スケール の順に行列を掛け合わせる
	MATRIX m = MGetIdent();// 単位行列を取得
	m = MMult(m, mRotY);
	m = MMult(m, mScale);
	m = MMult(m, mTrans);

	// モデルに行列をセット
	MV1SetMatrix(_iHandle, m);

	// モデルの描画
	MV1DrawModel(_iHandle);

	return true;
}

void Enemy::DebugRender() {
	// カプセルの当たり判定を描画
	DrawCapsule3D(
		_vCollisionBottom,_vCollisionTop,_fCollisionR,16,
		GetColor(255, 0, 0),GetColor(255, 255, 255),FALSE
	);

	// 移動範囲を描画
	{
		unsigned int color = GetColor(0, 0, 255);// 青
		DrawCircle3D(_vHomePos, _enemyParam.moveRadius, color, 16);
		DrawLine3D(_vPos, _vHomePos, color);// 現在位置から初期位置への線
	}

	// 索敵範囲を描画
	{
		unsigned int color = GetColor(0, 255, 0);// 緑
		int segments = 16;// 扇形の分割数

		// 角度計算
		auto halfAngleRad = DEGREE_TO_RADIAN * _enemyParam.visionAngle;// 視界の半分の角度のラジアン
		auto currentDirAngle = atan2f(_vDir.x, _vDir.z);// 向きベクトルから現在の角度を計算
		auto startAngle = currentDirAngle - halfAngleRad;// 扇形の左端
		auto endAngle = currentDirAngle + halfAngleRad;// 扇形の右端

		// 視界の左右の境界線を描画
		// 敵の中心から視界の端の点までのオフセットを計算
		VECTOR vLeftOffset = VGet(
			sinf(startAngle) * _enemyParam.visionRange, 0.0f, cosf(startAngle) * _enemyParam.visionRange
		);
		VECTOR vRightOffset = VGet(
			sinf(endAngle) * _enemyParam.visionRange, 0.0f, cosf(endAngle) * _enemyParam.visionRange
		);
		// 現在位置にオフセットを足して、ワールド座標上の点を計算
		VECTOR vLeftEdge = VAdd(_vPos, vLeftOffset);
		VECTOR vRightEdge = VAdd(_vPos, vRightOffset);
		// 線を描画
		DrawLine3D(_vPos, vLeftEdge, color);
		DrawLine3D(_vPos, vRightEdge, color);

		// 扇形の外周を線分で描画
		VECTOR vPrevPoint = vLeftEdge;// 左端の点から開始
		for (int i = 0; i <= segments; ++i) {
			// 現在の点の角度を計算
			auto ratio = static_cast<float>(i) / static_cast<float>(segments);// 全体の割合を計算
			auto angle = startAngle + (halfAngleRad * 2.0f * ratio);// 視界の左端の角度に割合分の角度を足す

			// 円周上の次の点を計算
			VECTOR vNextPoint = VAdd(_vPos, VGet(sinf(angle) * _enemyParam.visionRange, 0.0f, cosf(angle) * _enemyParam.visionRange));

			// 前の点と次の点を結ぶ線を描画
			DrawLine3D(vPrevPoint, vNextPoint, color);
			vPrevPoint = vNextPoint;// 次のループのために点を進める
		}
	}

	// 追跡中の各範囲の描画
	{
		if (_currentState && _currentState->IsChasing()) {
			// 攻撃可能範囲を描画
			unsigned int attackColor = GetColor(255, 0, 0);// 赤
			DrawCircle3D(_vPos, _enemyParam.attackRange, attackColor, 16);

			// 追跡限界範囲を描画
			unsigned int chaseColor = GetColor(255, 255, 0);// 黄
			DrawCircle3D(_vPos, _enemyParam.chaseLimitRange, chaseColor, 16);
		}
	}

	// デバッグ文字列の描画
	{
		// 敵を箱型と定義して、カメラに映っているか判定
		// 中心から半径と高さ分の範囲
		VECTOR vMin = VAdd(_vPos, VGet(-_fCollisionR, 0.0f, -_fCollisionR));
		VECTOR vMax = VAdd(_vPos, VGet(_fCollisionR, _fCollisionHeight, _fCollisionR));

		// CheckCameraViewClip_Box：カメラの視界外ならTRUEを返す
		if (CheckCameraViewClip_Box(vMin, vMax) == FALSE) {// 映っている時だけ
			VECTOR vScreenPos = ConvWorldPosToScreenPos(VAdd(_vPos, VGet(0, 150.0f, 0)));// 画面座標に変換(少し上に調整)
			int x = static_cast<int>(vScreenPos.x);
			int y = static_cast<int>(vScreenPos.y);
			int size = 16;// 改行用
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 255, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;

			// 状態名
			// ステートから名前を取得
			const char* stateName = "None";
			if (_currentState) {
				stateName = _currentState->GetName();
			}
			DrawFormatString(x, y, GetColor(255, 255, 0), "  state  = %s", stateName); y += size;
		}
	}
}



void Enemy::DrawCircle3D(VECTOR center, float radius, unsigned int color, int segments) {
	// 1区画当たりの角度(ラジアン)
	auto step = DX_TWO_PI_F / static_cast<float>(segments);

	// 最初の点を計算
	VECTOR vPrevPos;
	vPrevPos.x = center.x + sinf(0.0f) * radius;
	vPrevPos.y = center.y;// 高さは中心と同じ
	vPrevPos.z = center.z + cosf(0.0f) * radius;

	// 分割数分ループして点を計算し、線を描画していく
	for (int i = 0; i <= segments; i++) {
		auto angle = step * static_cast<float>(i);// 現在の角度を計算

		// 次の点を計算
		VECTOR vNextPos;
		vNextPos.x = center.x + sinf(angle) * radius;
		vNextPos.y = center.y;// 高さは中心と同じ
		vNextPos.z = center.z + cosf(angle) * radius;

		// 前の点と次の点を結ぶ線を描画
		DrawLine3D(vPrevPos, vNextPos, color);

		vPrevPos = vNextPos;// 次のループのために点を進める
	}
}



void Enemy::ChangeState(std::shared_ptr<EnemyState> newState) {
	if (_currentState) {
		_currentState->Exit(this);
	}
	_currentState = newState;
	if (_currentState) {
		_currentState->Enter(this);
	}
}

void Enemy::SetEnemyParam(const EnemyParam& param) {
	_enemyParam = param;

	// 視界のcos値を計算して設定
	auto rad = _enemyParam.visionAngle * DEGREE_TO_RADIAN;
	_enemyParam.visionCos = cosf(rad);
}

void Enemy::SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime) {
	auto bulletManager = _bulletManager.lock();// マネージャーが有効か確認
	if (bulletManager) {
		// タイプを設定して、発射リクエストをする
		bulletManager->Shoot(vStartPos, vDir, fRadius, fSpeed, lifeTime, _eCharaType);
	}
}

void Enemy::StartAttack(const EnemyAttackSettings& settings) {

	if (!_attackCollision) {// 攻撃コリジョンがなければ作成
		_attackCollision = std::make_shared<AttackBase>();
		_attackCollision->Initialize();
	}

	// 位置更新
	UpdateAttackTransform(settings);// 初期座標をセット

	// ステートごとの設定を反映
	_attackCollision->SetCapsuleAttackData(
		settings.vTopOffset,// top
		settings.vBottomOffset,// bottom
		settings.fRadius,// 半径
		settings.fDelay,// 発生遅延
		settings.fDuration,// 持続時間
		settings.fRecovery,// 後隙
		settings.fDamage,// ダメージ
		false// ヒットフラグ
	);

	// 攻撃開始
	_attackCollision->ProcessStartAttack();

	// AttackManagerに登録
	AttackManager::GetInstance().RegisterAttack(_attackCollision, ATTACK_OWNER_TYPE::ENEMY, settings.ownerId);
}

void Enemy::UpdateAttackTransform(const EnemyAttackSettings& settings) {
	if (!_attackCollision) return;

	// 敵の向きベクトルをラジアンに変換
	auto yaw = atan2f(_vDir.x, _vDir.z);

	MATRIX mRotY = MGetRotY(yaw);// Y軸回転行列を作成

	// ローカルオフセットを回転行列で変換
	VECTOR vTopOffsetRot = VTransform(settings.vTopOffset, mRotY);
	VECTOR vBottomOffsetRot = VTransform(settings.vBottomOffset, mRotY);

	// ワールド座標を計算
	VECTOR vTopPos = VAdd(_vPos, vTopOffsetRot);
	VECTOR vBottomPos = VAdd(_vPos, vBottomOffsetRot);

	// 攻撃コリジョンの座標だけ更新
	auto col = _attackCollision->GetAttackCollision();
	_attackCollision->SetCapsuleAttackData(
		vTopPos,// ワールド座標
		vBottomPos,
		col.attackColR,
		col.attackDelay,
		col.attackDuration,
		col.recovery,
		col.damage,
		col.isHit
	);
}

void Enemy::StopAttack() {
	if (!_attackCollision) return;
	// 攻撃停止
	_attackCollision->ProcessStopAttack();
}