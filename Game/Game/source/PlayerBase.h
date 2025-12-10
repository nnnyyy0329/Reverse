#pragma once
#include "CharaBase.h"

class PlayerBase : public CharaBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	virtual bool	Initialize();	// ‰Šú‰»
	virtual bool	Terminate();	// I—¹
	virtual bool	Process();		// XV
	virtual bool	Render();		// •`‰æ

	// “ü—Íó‘Ô‚ğİ’è‚·‚é
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
	}

protected:
	// “ü—Íó‘Ô
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
};

