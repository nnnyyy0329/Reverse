#pragma once
#include "GameObjectBase.h"

// キャラ状態列挙型
enum class CHARA_STATUS
{
	NONE,
	WAIT,
	WALK,
	FIRST_ATTACK,
	SECOND_ATTACK,
	THIRD_ATTACK,
	JUMP_UP,	
	JUMP_DOWN,	
	CROUCH_WAIT,
	CROUCH_WALK,
	DEATH,
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

	// ゲッターセッター
	
	// キャラの状態 
	CHARA_STATUS GetStatus() { return _eStatus; }		// 現在の状態を取得
	void SetStatus(CHARA_STATUS e) { _eStatus = e; }	// 現在の状態を設定

	// 当たり判定用
	VECTOR GetCollisionTop() { return _vCollisionTop; }		// 当たり判定の上端
	void SetCollisionTop(VECTOR v) { _vCollisionTop = v; }	// 当たり判定の上端

	VECTOR GetCollisionBottom() { return _vCollisionBottom; }		// 当たり判定の下端
	void SetCollisionBottom(VECTOR v) { _vCollisionBottom = v; }	// 当たり判定の下端

	float GetCollisionR() { return _fCollisionR; }		// 当たり判定の半径
	void SetCollisionR(float f) { _fCollisionR = f; }	// 当たり判定の半径

	// 基礎ステータス
	float GetMoveSpeed() { return _fMoveSpeed; }	// 移動速度
	void SetMoveSpeed(float f) { _fMoveSpeed = f; }	// 移動速度

	float GetDirSpeed() { return _fDirSpeed; }		// 回転速度
	void SetDirSpeed(float f) { _fDirSpeed = f; }	// 回転速度

	float GetLife() { return _fLife; }		// 体力
	void SetLife(float f) { _fLife = f; }	// 体力

	float GetGravity() { return _fGravity; }	// 重力
	void SetGravity(float f) { _fGravity = f; }	// 重力

protected:
	CHARA_STATUS _eStatus;		// キャラの状態
	CHARA_STATUS _eOldStatus;	// 前フレームのキャラの状態

	// 当たり判定用
	VECTOR _vCollisionTop;		// 当たり判定の上端
	VECTOR _vCollisionBottom;	// 当たり判定の下端
	float _fCollisionR;			// 当たり判定の半径

	// 基礎ステータス
	float _fMoveSpeed;			// 移動速度
	float _fDirSpeed;			// 回転速度
	float _fLife;				// 体力
	float _fGravity;			// 重力	

};

