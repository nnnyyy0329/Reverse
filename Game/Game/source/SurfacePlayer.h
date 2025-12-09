#pragma once
#include "PlayerBase.h"

class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool	Initialize();	// ‰Šú‰»
	virtual bool	Terminate();	// I—¹
	virtual bool	Process();		// XV
	virtual bool	Render();		// •`‰æ

protected:


};

