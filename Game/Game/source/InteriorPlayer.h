#pragma once
#include "PlayerBase.h"

class InteriorPlayer : public PlayerBase
{
public:
	InteriorPlayer();
	virtual ~InteriorPlayer();

	virtual bool	Initialize();	// ‰Šú‰»
	virtual bool	Terminate();	// I—¹
	virtual bool	Process();		// XV
	virtual bool	Render();		// •`‰æ

protected:


};

