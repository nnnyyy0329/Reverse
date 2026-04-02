/*****************************************
* file	 AbilityBase.h
* brief  能力の基底クラス
* author 成田 悠真
* date   2025/12/27
******************************************/

#pragma once
#include "appframe.h"

// 能力選択のインデックス定数
namespace AbilityConstants
{
	constexpr int ABILITY_SELECTION_SURFACE = 0;	// 表プレイヤー選択
	constexpr int ABILITY_SELECTION_BULLET = 1;		// 弾プレイヤー選択
	constexpr int ABILITY_SELECTION_INTERIOR = 2;	// 裏プレイヤー選択
}

// 能力タイプの定義
enum class ABILITY_TYPE
{
	NONE,
	SURFACE_PLAYER,    ///< 表プレイヤー
	INTERIOR_PLAYER,   ///< 裏プレイヤー
	BULLET_PLAYER,     ///< 弾プレイヤー
	_EOT_
};

/// @brief 選択インデックスを能力タイプに変換する関数
///
/// @param selection 選択インデックス
/// 
/// @return 選択インデックスに対応する能力タイプ、無効な選択の場合は ABILITY_TYPE::NONE を返す
inline ABILITY_TYPE ConvertSelectionToAbility(int selection)
{
	// 選択インデックスに応じて能力タイプを返す
	switch(selection)
	{
		case AbilityConstants::ABILITY_SELECTION_SURFACE:	return ABILITY_TYPE::SURFACE_PLAYER;
		case AbilityConstants::ABILITY_SELECTION_BULLET:	return ABILITY_TYPE::BULLET_PLAYER;
		case AbilityConstants::ABILITY_SELECTION_INTERIOR:	return ABILITY_TYPE::INTERIOR_PLAYER;

		default:	return ABILITY_TYPE::NONE;
	}
}

/// @brief 能力の基底クラス
class AbilityBase
{
public:

	AbilityBase();
	virtual ~AbilityBase();


	/* 基本関数 */

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

protected:

	ABILITY_TYPE _eAbilityType;	// 能力タイプ
};