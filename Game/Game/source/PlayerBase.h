#pragma once
#include "CharaBase.h"

// 状態列挙型
enum class PLAYER_STATUS
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

class PlayerBase : public CharaBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}

	// キャラの状態 
	PLAYER_STATUS GetStatus() { return _ePlayerStatus; }		// 現在の状態を取得
	void SetStatus(PLAYER_STATUS e) { _ePlayerStatus = e; }	// 現在の状態を設定


protected:

	PLAYER_STATUS _ePlayerStatus;		// キャラの状態
	PLAYER_STATUS _eOldPlayerStatus;	// 前フレームのキャラの状態

	// 入力状態
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;
};

