#pragma once
#include "appframe.h"

class GameObjectBase
{
public:
	GameObjectBase();
	virtual ~GameObjectBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	// ゲッターセッター
	VECTOR GetPos() { return _vPos; }
	void SetPos(VECTOR v) { _vPos = v; }

	VECTOR GetOldPos() { return _vOldPos; }
	void SetOldPos(VECTOR v) { _vOldPos = v; }

	VECTOR GetRad() { return _vDir; }
	void SetRad(VECTOR v) { _vDir = v; }

	VECTOR GetScale() { return _vScale; }
	void SetScale(VECTOR v) { _vScale = v; }

	VECTOR GetMove() { return _vMove; }
	void SetMove(VECTOR v) { _vMove = v; }

protected:
	VECTOR _vPos;		// 位置
	VECTOR _vOldPos;	// 前フレームの位置
	VECTOR _vDir;		// 回転
	VECTOR _vScale;		// 拡縮
	VECTOR _vMove;		// 移動量

	int _iHandle;		// グラフィックハンドル
	int _iAttachIndex;	// アタッチインデックス
	float _fTotalTime;	// 総経過時間
	float _fPlayTime;	// 再生時間

private:


};

