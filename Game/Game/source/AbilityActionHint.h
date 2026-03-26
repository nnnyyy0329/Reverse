#pragma once
#include "appframe.h"
#include "AbilityBase.h"

// 能力入力ヒント表示設定用の名前空間
namespace AbilityHintConfig
{
	constexpr int DRAW_Y = 750;			// 描画位置Y
	constexpr int DRAW_OFFSET_Y = -10;	// 描画位置Y
}

// アクションヒント表示設定用の名前空間
namespace ActionHintConfig
{
	constexpr int DRAW_X = 1679;		// 描画位置X
	constexpr int DRAW_Y = 910;			// 描画位置Y
	constexpr int DRAW_OFFSET_X = 0;	// 描画位置オフセットX
	constexpr int DRAW_OFFSET_Y = 90;	// 描画位置オフセットY
}

// バンパー入力ヒント表示設定用の名前空間
namespace BumperHintConfig
{
	constexpr int DRAW_X = 1320;		// 描画位置X
	constexpr int DRAW_Y = 1020;			// 描画位置Y
	constexpr int DRAW_OFFSET_X = 60;	// 描画位置オフセットX
	constexpr int DRAW_OFFSET_Y = 0;	// 描画位置オフセットY
}

/// @brief アビリティアクションヒントクラス
class AbilityActionHint
{
public:

	AbilityActionHint();
	virtual ~AbilityActionHint();



	/* 基本関数 */

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();



	/* アクションヒント表示関係関数 */

	/// @brief アクションヒントの表示
	///
	/// @param abilityType アビリティの種類
	/// @param selectGraphSenterX 選択グラフの中心X座標
	/// @param secondSelectGraphCenterX 二番目に選択されたグラフの中心X座標
	void AbilityInputHintRender(ABILITY_TYPE abilityType, float selectGraphSenterX, float secondSelectGraphCenterX);

	/// @brief 各アクションヒントの表示
	void ActionInputHintRender();

	/// @brief バンパー入力ヒントの表示
	void BumperInputHintRender();

private:


protected:

	int _iHandleAbsorbAction;			// 吸収攻撃アクションヒントのハンドル
	int _iHandleAoeAction;				// 範囲攻撃アクションヒントのハンドル
	int _iHandlePunchAction;			// 打撃攻撃アクションヒントのハンドル
	int _iHandlePiercingBulletAction;	// 貫通弾アクションヒントのハンドル
	int _iHandleNormalBulletAction;		// 通常弾アクションヒントのハンドル

	int _iHandleDodgeAction;			// 回避アクションヒントのハンドル
	int _iHandleTransCancelAction;		// 変身解除アクションヒントのハンドル

	int _iHandleButtonA;				// Aボタンアクションヒントのハンドル
	int _iHandleButtonY;				// Yボタンアクションヒントのハンドル
	int _iHandleDodgeRB;				// RBボタン回避アクションヒントのハンドル
	int _iHandleLB;						// LBボタンアクションヒントのハンドル

};

