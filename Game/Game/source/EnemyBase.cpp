#include "EnemyBase.h"

namespace {
	constexpr auto DEFAULT_MOVE_SPEED = 2.0f;// 敵の基本移動速度

	constexpr auto IDLE_TIME = 120.0f;// 待機状態の時間
	constexpr auto MOVE_TIME = 120.0f;// 自動移動状態の時間

	constexpr auto GO_HOME_DISTANCE = 300.0f;// 初期位置に戻る距離

	constexpr auto COLLISION_RADIUS = 30.0f;// 敵の当たり判定半径
	constexpr auto COLLISION_HEIGHT = 100.0f;// 敵の当たり判定高さ
}

EnemyBase::EnemyBase(){
	// モデル読み込み
	_iHandle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_iAttachIndex = MV1AttachAnim(_iHandle, 0, -1, FALSE);
	_fTotalTime = MV1GetAttachAnimTotalTime(_iHandle, _iAttachIndex);
	_fPlayTime = 0.0f;

	_vPos = VGet(300.0f, 0.0f, 0.0f);// テストで調整
	_vDir = VGet(0.0f, 0.0f, 1.0f);// 前方を向いておく
	_fMoveSpeed = DEFAULT_MOVE_SPEED;
	_eCurrentState = ENEMY_STATE::IDLE;
	_eOldState = ENEMY_STATE::NONE;
	_fStateTimer = 0.0f;
	_vHomePos = _vPos;

	// 当たり判定用(カプセル)
	_fCollisionR = COLLISION_RADIUS;
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);
	_vCollisionTop = VGet(0.0f, COLLISION_HEIGHT, 0.0f);
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

	_vMove = VGet(0.0f, 0.0f, 0.0f);// 毎フレーム移動量リセット

	// 現在の状態に応じて処理を分岐
	switch (_eCurrentState) {
	case ENEMY_STATE::IDLE: EnemyIdleProcess(); break;
	case ENEMY_STATE::MOVE: EnemyMoveProcess(); break;
	case ENEMY_STATE::CHASE: EnemyChaseProcess(); break;
	case ENEMY_STATE::ATTACK: EnemyAttackProcess(); break;
	}

	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// カプセルに座標を対応させる
	_vCollisionBottom = _vPos;
	_vCollisionTop = VAdd(_vCollisionBottom, VGet(0.0f, COLLISION_HEIGHT, 0.0f));// 高さ分ずらす

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
		GetColor(255, 0, 0),GetColor(255, 255, 255),FALSE);
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

void EnemyBase::EnemyChaseProcess() {
}

void EnemyBase::EnemyAttackProcess() {
}