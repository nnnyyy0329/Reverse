#pragma once
#include "GameObjectBase.h"

enum class CHARA_TYPE
{
	NONE,
	SURFACE_PLAYER,
	INTERIOR_PLAYER,

	// 敵もここに追加

	_EOT_,
};

class CharaBase : public GameObjectBase
{
public:
	CharaBase();
	virtual ~CharaBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	/*****ゲッターセッター*****/
	// 当たり判定用
	VECTOR GetCollisionTop() { return _vCollisionTop; }		// 当たり判定の上端
	void SetCollisionTop(VECTOR v) { _vCollisionTop = v; }	// 当たり判定の上端

	VECTOR GetCollisionBottom() { return _vCollisionBottom; }		// 当たり判定の下端
	void SetCollisionBottom(VECTOR v) { _vCollisionBottom = v; }	// 当たり判定の下端

	float GetCollisionR() { return _fCollisionR; }		// 当たり判定の半径
	void SetCollisionR(float f) { _fCollisionR = f; }	// 当たり判定の半径

	float GetCollisionHeight() { return _fCollisionHeight; }// 当たり判定の高さ
	void SetCollisionHeight(float f) { _fCollisionHeight = f; }// 当たり判定の高さ

	float GetColSubY() { return _colSubY; }		// コリジョン判定時のY補正(腰位置）

	// 基礎ステータス
	float GetMoveSpeed() { return _fMoveSpeed; }	// 移動速度
	void SetMoveSpeed(float f) { _fMoveSpeed = f; }	// 移動速度

	float GetDirSpeed() { return _fDirSpeed; }		// 回転速度
	void SetDirSpeed(float f) { _fDirSpeed = f; }	// 回転速度

	float GetLife() { return _fLife; }		// 体力
	void SetLife(float f) { _fLife = f; }	// 体力

	float GetGravity() { return _fGravity; }	// 重力
	void SetGravity(float f) { _fGravity = f; }	// 重力

	CHARA_TYPE GetCharaType() { return _eCharType; }					// キャラタイプ取得
	void SetCharaType(CHARA_TYPE charaType) { _eCharType = charaType; }	// キャラタイプ設定

protected:
	// キャラカプセルの当たり判定用
	VECTOR _vCollisionTop;		// 当たり判定の上端
	VECTOR _vCollisionBottom;	// 当たり判定の下端
	float _fCollisionR;			// 当たり判定の半径
	float _fCollisionHeight;	// 当たり判定の高さ
	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// デバッグ用
	bool	_bViewCollision;

	// 基礎ステータス
	float _fMoveSpeed;			// 移動速度
	float _fDirSpeed;			// 回転速度
	float _fLife;				// 体力
	float _fGravity;			// 重力	

	// キャラタイプ
	CHARA_TYPE _eCharType;
};

