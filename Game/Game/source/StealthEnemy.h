#pragma once
#include "EnemyBase.h"

class StealthEnemy : public EnemyBase
{
public:
	StealthEnemy();
	virtual ~StealthEnemy();

	virtual bool	Initialize()override;	// ‰Šú‰»
	virtual bool	Terminate()override;	// I—¹
	virtual bool	Process()override;		// XV
	virtual bool	Render()override;		// •`‰æ

protected:


};

