// 担当 : 成田

#pragma once
#include "appframe.h"

// 能力の種類
enum class ABILITY_TYPE
{
	NONE,
	_EOT_,
};

class AbilityBase
{
public:
	AbilityBase();
	virtual ~AbilityBase();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	// ゲッターセッター
	ABILITY_TYPE GetAbilityType() const { return _eAbilityType; }

protected:
	ABILITY_TYPE _eAbilityType;
};