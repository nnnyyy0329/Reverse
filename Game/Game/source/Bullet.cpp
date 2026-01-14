#include "Bullet.h"

Bullet::Bullet() 
	: _lifeTimer(0) 
{
	_fCollisionR = 5.0f;
	_fCollisionHeight = 10.0f;
}

Bullet::~Bullet() {
}

bool Bullet::Initialize() {
	return true;
}

bool Bullet::Terminate() {
	return true;
}

bool Bullet::Process() {
	// 移動処理
	_vOldPos = _vPos;// 前フレームの位置を保存
	_vPos = VAdd(_vPos, _vMove);// 位置更新

	// カプセルに座標を対応させる
	_vCollisionBottom = _vPos;
	_vCollisionTop = VAdd(_vPos, VGet(0.0f, _fCollisionHeight, 0.0f));

	// 寿命チェック
	_lifeTimer--;
	if (_lifeTimer <= 0) {
		return false;// リストから削除する
	}

	return true;// 生存している
}

bool Bullet::Render() {
	// 当たり判定のカプセルを描画
	DrawCapsule3D(
		_vCollisionBottom, _vCollisionTop, _fCollisionR, 16,
		GetColor(0, 255, 255), GetColor(255, 255, 255), FALSE
	);

	return true;
}

void Bullet::DebugRender() {
}

void Bullet::Activate(VECTOR vStartPos, VECTOR vDir, float fSpeed, int lifeTime) {
	_vPos = vStartPos;
	_vOldPos = vStartPos;// 前フレームの位置も初期化

	_vDir = VNorm(vDir);// 向きを正規化
	_fMoveSpeed = fSpeed;
	_lifeTimer = lifeTime;

	_vMove = VScale(_vDir, _fMoveSpeed);// 移動量を設定

	// カプセルの初期位置
	_vCollisionBottom = _vPos;
	_vCollisionTop = VAdd(_vPos, VGet(0.0f, _fCollisionHeight, 0.0f));
}
