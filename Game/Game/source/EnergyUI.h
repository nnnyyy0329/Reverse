#pragma once
#include "appframe.h"

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
    int _iEnergyFrameHandle;
    int _iEnergyHandle1;
    int _iEnergyHandle2;
    int _iEnergyHandle3;

};

