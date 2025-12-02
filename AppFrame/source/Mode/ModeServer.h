#pragma once

#include <list>
#include "ModeBase.h"


class	ModeServer
{
	typedef	std::list<ModeBase*>		lstModeBase;

public:
	ModeServer();
	virtual	~ModeServer();
	static ModeServer		*_lpInstance;
	static ModeServer*GetInstance() { return (ModeServer*)_lpInstance; }

	int Add(ModeBase *mode, int layer, const char *name);		// 登録はするが、一度メインを回さないといけない
	int Del(ModeBase *mode);		// 削除予約
	ModeBase *Get(int uid);
	ModeBase *Get(const char *name);
	int GetId(ModeBase* mode);
	int GetId(const char *name);
	const char *GetName(ModeBase* mode);
	const char *GetName(int uid);
	void Clear();
	int	LayerTop() { return INT32_MAX; }

	static bool modeSort(const ModeBase *x, const ModeBase *y) {
		return x->_layer < y->_layer;
	}

	int ProcessInit();	// プロセスを回すための初期化
	int Process();		// レイヤーの上の方から処理
	int ProcessFinish();		// プロセスを回した後の後始末

	int RenderInit();		// 描画を回すための初期化
	int Render();		// レイヤーの下の方から処理
	int RenderFinish();		// 描画を回した後の後始末

	int SkipProcessUnderLayer();		// 現Processで、今処理しているレイヤーより下のレイヤーは、処理を呼ばない
	int SkipRenderUnderLayer();			// 現Processで、今処理しているレイヤーより下のレイヤーは、描画を呼ばない
	int PauseProcessUnderLayer();		// 現Processで、今処理しているレイヤーより下のレイヤーは、時間経過を止める

private:
	int Release(ModeBase *mode);		// 削除＆delete
	bool IsDelRegist(ModeBase *mode);	// 削除予約されているか？
	bool IsAdd(ModeBase *mode);		// リストにあるか？


public:


private:
	lstModeBase		_vMode;			// モードリスト
	int				_uid_count;		// uidカウンタ
	lstModeBase		_vModeAdd;		// 追加予約
	lstModeBase		_vModeDel;		// 削除予約

	ModeBase		*_nowMode;		// 現在呼び出し中のモード
	ModeBase		*_skipProcessMode;	// このモードより下はProcessを呼ばない
	ModeBase		*_skipRenderMode;	// このモードより下はRenderを呼ばない
	ModeBase		*_pauseProcessMode;	// このモードより下はProcess時に時間経過をさせない

};
