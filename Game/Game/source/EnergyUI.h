/*****************************************
* file	 EnergyUI.h
* brief  エネルギーUIクラス
* author 成田 悠真
* date   2026/01/25
******************************************/

#pragma once
#include "appframe.h"

/// @brief エネルギーUIクラス
class EnergyUI
{
public:

	EnergyUI();
	virtual ~EnergyUI();


	/* 基本関数 */

    bool Initialize();
    bool Terminate();
    bool Process();
    bool Render();


	/* エネルギーUI表示関係関数 */

	/// @brief エネルギーフレーム表示関数
	void EnergyFrameRender();

	/// @brief ゲージ表示比率計算関数
	void GaugeRatioCalculation();
	
	/// @brief エネルギーゲージ表示関数
	///
	/// @param ratio ゲージ表示比率
    void EnergyGaugeRender(float ratio);

protected:

	int _iEnergyFrameHandle;    // エネルギーフレーム画像ハンドル
	int _iEnergyHandle1;        // エネルギーゲージ画像ハンドル1
	int _iEnergyHandle2;        // エネルギーゲージ画像ハンドル2
	int _iEnergyHandle3;        // エネルギーゲージ画像ハンドル3

	int _drawX; // 描画位置X
	int _drawY; // 描画位置Y
};

