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

protected:

};

