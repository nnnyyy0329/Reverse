#pragma once

#include	<string>

class ModeServer;


class		ModeBase
{
public:
	ModeBase();
	virtual ~ModeBase();

	virtual bool	Initialize();
	virtual bool	Terminate();
	virtual bool	Process();
	virtual bool	Render();


public:
	int	GetModeCount() { return _cntMode; }			// このモードが始まってからのカウンタ
	unsigned long GetModeTm() { return _tmMode; }	// このモードが始まってからの時間ms
	unsigned long GetStepTm() { return _tmStep; }	// 前フレームからの経過時間ms

	void SetCallPerFrame(int frame) { _callPerFrame = _callPerFrame_cnt = frame; }	// 何フレームに1回Process()を呼ぶか(def:1)
	void SetCallOfCount(int count) { _callOfCount = count; }		// 1回の呼び出しに何回Process()を呼ぶか(def:1)
	int GetCallPerFrame() { return _callPerFrame; }
	int GetCallOfCount() { return _callOfCount; }


private:
	friend	ModeServer;
	// ModeServer用
	std::string		_szName;
	int				_uid;
	int				_layer;

	void StepTime(unsigned long tmNow);
	void StepCount();

	// モード内処理用
	int		_cntMode;	// 本モードに入ってからのカウンタ, 0スタート
	unsigned long	_tmMode;	// 本モードに入ってからの時間。ms
	unsigned long	_tmStep;	// 前フレームからの経過時間。ms
	unsigned long	_tmModeBase;	// このモードが始まった時間。ms
	unsigned long	_tmPauseBase;	// ポーズ中のベース時間。ms
	unsigned long	_tmPauseStep;	// ポーズ中の積算時間。ms 動作中、この値を _tmMode に用いる
	unsigned long	_tmOldFrame;	// 前フレームの時間。ms

	// CallPerFrame / CallOfCount用
	int		_callPerFrame, _callPerFrame_cnt;
	int		_callOfCount;

};


