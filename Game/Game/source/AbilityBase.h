// 担当 : 成田

#pragma once
#include "appframe.h"

// 能力タイプの定義
enum class ABILITY_TYPE
{
	NONE,
	SURFACE_PLAYER,    // 表プレイヤー
	INTERIOR_PLAYER,   // 裏プレイヤー
	BULLET_PLAYER,     // 弾プレイヤー
	_EOT_
};

// 能力選択から実際の能力タイプへの変換
inline ABILITY_TYPE ConvertSelectionToAbility(int selection)
{
	switch(selection)
	{
		case 0:		return ABILITY_TYPE::SURFACE_PLAYER;
		case 1:		return ABILITY_TYPE::BULLET_PLAYER;
		case 2:		return ABILITY_TYPE::INTERIOR_PLAYER;

		default:	return ABILITY_TYPE::NONE;
	}
}

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


protected:
	ABILITY_TYPE _eAbilityType;
};