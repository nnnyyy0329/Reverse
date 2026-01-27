#include "Bullet.h"

Bullet::Bullet() 
	: _lifeTimer(0),
	_eShooterType(CHARA_TYPE::NONE)
{
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

	// 寿命チェック
	_lifeTimer--;
	if (_lifeTimer <= 0) {
		return false;// リストから削除する
	}

	return true;// 生存している
}

bool Bullet::Render() {
	// 当たり判定のsphereを描画
	// 中心座標、半径、分割数、ディフューズ、スペキュラ、塗りつぶすか
	DrawSphere3D(
		_vPos, _fCollisionR, 8, GetColor(0, 255, 255), GetColor(255, 255, 255), FALSE
	);

	return true;
}

void Bullet::DebugRender() {
}

void Bullet::CollisionRender() {
}

void Bullet::Activate(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime, CHARA_TYPE type) {
	_vPos = vStartPos;
	_vOldPos = vStartPos;// 前フレームの位置も初期化

	_vDir = VNorm(vDir);// 向きを正規化
	_fCollisionR = fRadius;
	_fMoveSpeed = fSpeed;
	_lifeTimer = lifeTime;
	_eShooterType = type;

	_vMove = VScale(_vDir, _fMoveSpeed);// 移動量を設定
}
