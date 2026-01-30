#pragma once
#include "appframe.h"

// エネルギーUIクラス
class EnergyUI
{
public:
	EnergyUI();
	virtual ~EnergyUI();

    bool Initialize();
    bool Terminate();
    bool Process();
    bool Render();

    void EnergyFrameRender();             // エネルギーフレーム表示関数
    void GaugeRatioCalculation();         // ゲージ表示比率計算関数
    void EnergyGaugeRender(float ratio);  // エネルギーゲージ表示関数

protected:
	int _iEnergyFrameHandle;    // エネルギーフレーム画像ハンドル
	int _iEnergyHandle1;        // エネルギーゲージ画像ハンドル1
	int _iEnergyHandle2;        // エネルギーゲージ画像ハンドル2
	int _iEnergyHandle3;        // エネルギーゲージ画像ハンドル3

	int _drawX; // 描画位置X
	int _drawY; // 描画位置Y
};

