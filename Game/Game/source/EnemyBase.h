#pragma once
#include "CharaBase.h"

class EnemyBase : public CharaBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual bool	Initialize();	// ‰Šú‰»
	virtual bool	Terminate();	// I—¹
	virtual bool	Process();		// XV
	virtual bool	Render();		// •`‰æ

protected:

};

