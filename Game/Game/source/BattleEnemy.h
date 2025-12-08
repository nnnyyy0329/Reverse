#pragma once
#include "EnemyBase.h"

class BattleEnemy : public EnemyBase
{
public:
	BattleEnemy();
	virtual ~BattleEnemy();

	virtual bool	Initialize()override;	// ‰Šú‰»
	virtual bool	Terminate()override;	// I—¹
	virtual bool	Process()override;		// XV
	virtual bool	Render()override;		// •`‰æ

protected:


};

