#pragma once
#include "appframe.h"

// スタミナUIクラス
class StaminaUI
{
public:
	StaminaUI();
	virtual ~StaminaUI();
	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	void StaminaFrameRender();				// スタミナフレーム表示関数
	void GaugeRatioCalculation();			// ゲージ表示比率計算関数
	void StaminaGaugeRender(float ratio);	// スタミナゲージ表示関数

protected:
	int _iStaminaFrameHandle;    // スタミナフレーム画像ハンドル
	int _iStaminaHandle;        // スタミナゲージ画像ハンドル

	int _drawStaminaBarX;		// スタミナバーの描画位置X
	int _drawStaminaBarY;		// スタミナバーの描画位置Y
	int _drawStaminaBarFrameX;	// スタミナバーフレームの描画位置X
	int _drawStaminaBarFrameY;	// スタミナバーフレームの描画位置Y
};

