/*****************************************
* file   StaminaUI.h
* brief  スタミナUIクラス
* author 成田 悠真
* date   2026/02/06
******************************************/

#pragma once
#include "appframe.h"

// スタミナUI表示設定用定数
namespace StaminaRenderConfig
{
	// 画像表示用
	const int DRAW_BAR_FRAME_X	= 216;
	const int DRAW_BAR_FRAME_Y	= 1000;
	const int DRAW_OFFSET_X		= 15;
	const int DRAW_OFFSET_Y		= 5;
}

/// @brief スタミナUIクラス
class StaminaUI
{
public:

	StaminaUI();
	virtual ~StaminaUI();


	/* 基本関数 */

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();


	/* スタミナUI表示関係関数 */

	/// @brief スタミナフレーム表示関数
	void StaminaFrameRender();			

	/// @brief ゲージ表示比率計算関数
	void GaugeRatioCalculation();			
	
	/// @brief スタミナゲージ表示関数
	///
	/// @param ratio ゲージ表示比率
	void StaminaGaugeRender(float ratio);	

protected:

	int _iStaminaFrameHandle;	// スタミナフレーム画像ハンドル
	int _iStaminaHandle;// スタミナゲージ画像ハンドル

	int _drawStaminaBarX;		// スタミナバーの描画位置X
	int _drawStaminaBarY;		// スタミナバーの描画位置Y
	int _drawStaminaBarFrameX;	// スタミナバーフレームの描画位置X
	int _drawStaminaBarFrameY;	// スタミナバーフレームの描画位置Y
};

