#include "EnemyBase.h"

namespace {
	constexpr auto DEFAULT_MOVE_SPEED = 2.0f;// 敵の基本移動速度

	constexpr auto IDLE_TIME = 120.0f;// 待機状態の時間
	constexpr auto MOVE_TIME = 180.0f;// 自動移動状態の時間
	constexpr auto DETECT_TIME = 30.0f;// 発見状態の時間

	constexpr auto GO_HOME_DISTANCE = 300.0f;// 初期位置に戻る距離

	constexpr auto COLLISION_RADIUS = 30.0f;// 敵の当たり判定半径
	constexpr auto COLLISION_HEIGHT = 100.0f;// 敵の当たり判定高さ

	constexpr auto VISION_RANGE = 250.0f;// 敵の索敵距離
	constexpr auto VISION_ANGLE = 60.0f;// 敵の視界の角度(半分)
}

EnemyBase::EnemyBase(){
	// モデル読み込み
	_iHandle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 0, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	_vPos = VGet(230.0f, 57.0f, 0.0f);// テストで調整
	_vOldPos = _vPos;
	_vDir = VGet(0.0f, 0.0f, 1.0f);// 前方を向いておく
	_fMoveSpeed = DEFAULT_MOVE_SPEED;
	_eCurrentState = ENEMY_STATE::IDLE;
	_eOldState = ENEMY_STATE::NONE;
	_fStateTimer = 0.0f;
	_vHomePos = _vPos;

	// 腰位置の設定
	_colSubY = 40.f;

	// 当たり判定用(カプセル)
	_fCollisionR = COLLISION_RADIUS;
	_fCollisionHeight = COLLISION_HEIGHT;
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionTop = VGet(0.0f, _fCollisionHeight, 0.0f);

	// 索敵関連
	_fVisionRange = VISION_RANGE;
	SetVisionAngle(VISION_ANGLE);// 視界角度設定
}

EnemyBase::~EnemyBase() {
}

bool EnemyBase::Initialize() {
	return true;
}

bool EnemyBase::Terminate() {
	return true;
}

bool EnemyBase::Process() {
	_fPlayTime += 0.5f;// 再生時間を進める
	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if (_fPlayTime >= _fTotalTime) {
		_fPlayTime = 0.0f;
	}

	_vOldPos = _vPos;// 前フレームの位置を保存
	_eOldState = _eCurrentState;// 前フレームの状態を保存

	_vMove = VGet(0.0f, 0.0f, 0.0f);// 毎フレーム移動量リセット

	// 現在の状態に応じて処理を分岐
	switch (_eCurrentState) {
	case ENEMY_STATE::IDLE: EnemyIdleProcess(); break;
	case ENEMY_STATE::MOVE: EnemyMoveProcess(); break;
	case ENEMY_STATE::DETECT: EnemyDetectProcess(); break;
	case ENEMY_STATE::CHASE: EnemyChaseProcess(); break;
	case ENEMY_STATE::ATTACK: EnemyAttackProcess(); break;
	}

	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// カプセルに座標を対応させる
	_vCollisionBottom = VAdd(_vPos, VGet(0, _fCollisionR, 0));// 半径分ずらして中心位置に
	_vCollisionTop = VAdd(_vPos, VGet(0, _fCollisionHeight - _fCollisionR, 0));// 高さ分ずらす

	return true;
}

bool EnemyBase::Render() {
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

void EnemyBase::DebugRender() {
	// カプセルの当たり判定を描画
	DrawCapsule3D(
		_vCollisionBottom,_vCollisionTop,_fCollisionR,16,
		GetColor(255, 0, 0),GetColor(255, 255, 255),FALSE
	);

	// 索敵範囲を描画
	{
		unsigned int color = GetColor(0, 255, 0);// 緑
		int segments = 16;// 扇形の分割数

		// 角度計算
		auto halfAngleRad = DEGREE_TO_RADIAN * _fVisionAngle;// 視界の半分の角度のラジアン
		auto currentDirAngle = atan2f(_vDir.x, _vDir.z);// 向きベクトルから現在の角度を計算
		auto startAngle = currentDirAngle - halfAngleRad;// 扇形の左端
		auto endAngle = currentDirAngle + halfAngleRad;// 扇形の右端

		// 視界の左右の境界線を描画
		// 敵の中心から視界の端の点までのオフセットを計算
		VECTOR vLeftOffset = VGet(
			sinf(startAngle) * _fVisionRange, 0.0f, cosf(startAngle) * _fVisionRange
		);
		VECTOR vRightOffset = VGet(
			sinf(endAngle) * _fVisionRange, 0.0f, cosf(endAngle) * _fVisionRange
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
			VECTOR vNextPoint = VAdd(_vPos, VGet(sinf(angle) * _fVisionRange, 0.0f, cosf(angle) * _fVisionRange));

			// 前の点と次の点を結ぶ線を描画
			DrawLine3D(vPrevPoint, vNextPoint, color);
			vPrevPoint = vNextPoint;// 次のループのために点を進める
		}
	}

	{
		int x = 0, y = 32, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(255, 255, 0), "Enemy:"); y += size;
		DrawFormatString(x, y, GetColor(255, 255, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
	}
}

// 敵の状態変更
void EnemyBase::ChangeState(ENEMY_STATE newState) {
	if (_eCurrentState == newState) return;// 同じ状態なら何もしない

	_eOldState = _eCurrentState;// 前の状態を保存
	_eCurrentState = newState;// 新しい状態に変更

	// 状態変更時の一回限りの処理をここに書く
	_fStateTimer = 0.0f;// 状態経過時間リセット
}

// 待機状態の処理
void EnemyBase::EnemyIdleProcess() {
	_fStateTimer++;// タイマーを進める

	if (_fStateTimer >= IDLE_TIME) {// 待機時間を超えたら
		VECTOR vToHome = VSub(_vHomePos, _vPos);// 初期位置へのベクトルを計算
		auto distFromHome = VSize(vToHome);// 初期位置からの距離を計算

		if (distFromHome > GO_HOME_DISTANCE) {// 初期位置から離れすぎなら
			_vDir = VNorm(vToHome);// 初期位置の方向を向きにする
		}
		else {
			// ランダムな方向を決める
			auto angle = static_cast<float>(GetRand(359)) * DEGREE_TO_RADIAN;// ランダムな角度をラジアンで計算
			_vDir = VGet(cosf(angle), 0.0f, sinf(angle));// XZ平面の角度の方向にある点のベクトルを計算
		}

		ChangeState(ENEMY_STATE::MOVE);// 自動移動状態へ
	}
}

// 自動移動状態の処理
void EnemyBase::EnemyMoveProcess() {
	_fStateTimer++;// タイマーを進める

	_vMove = VAdd(_vMove, VScale(_vDir, _fMoveSpeed));// 移動量を更新

	if (_fStateTimer >= MOVE_TIME) {// 自動移動時間を超えたら
		ChangeState(ENEMY_STATE::IDLE);// 待機状態へ
	}
}

// 発見状態の処理
void EnemyBase::EnemyDetectProcess() {
	_fStateTimer++;// タイマーを進める

	_vMove = VGet(0.0f, 0.0f, 0.0f);// その場で立ち止まる演出

	if (_fStateTimer >= DETECT_TIME) {// 発見時間を超えたら
		ChangeState(ENEMY_STATE::CHASE);// 追跡状態へ
	}
}

// 追跡状態の処理
void EnemyBase::EnemyChaseProcess() {
	if (!_targetPlayer) {// 範囲外に出たなどでターゲットがいなくなったら
		ChangeState(ENEMY_STATE::IDLE);// 待機状態へ
	}

	// ターゲットの方向を向く
	_vDir = VNorm(VSub(_targetPlayer->GetPos(), _vPos));// ターゲットへの単位ベクトルを計算
	_vMove = VScale(_vDir, _fMoveSpeed);// 移動量を更新
}

void EnemyBase::EnemyAttackProcess() {
}

void EnemyBase::SetVisionAngle(float angle) {
	_fVisionAngle = angle;
	// 判定で使うためあらかじめ計算しておく
	_fVisionCosAngle = cosf(DEGREE_TO_RADIAN * angle);
}

void EnemyBase::OnDetectPlayer(std::shared_ptr<CharaBase> target) {
	if (!target) return;

	// ここに遷移するかの条件をいれる

	// 発見した瞬間の処理
	_targetPlayer = target;// 追跡対象をセット
	ChangeState(ENEMY_STATE::DETECT);// まず発見状態へ
}