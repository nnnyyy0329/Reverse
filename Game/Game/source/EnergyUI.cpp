#include "EnergyUI.h"
#include "EnergyManager.h"

namespace
{
	// 画像表示用
	const int DRAW_X = 0;
	const int DRAW_Y = 0;
	const int DRAW_OFFSET_X = 137;
	const int DRAW_OFFSET_Y = 243;
}

EnergyUI::EnergyUI()
{
	_iEnergyFrameHandle = ResourceServer::GetInstance()->GetHandle("energyFrame");
	_iEnergyHandle1 = ResourceServer::GetInstance()->GetHandle("energy1");
	_iEnergyHandle2 = ResourceServer::GetInstance()->GetHandle("energy2");
	_iEnergyHandle3 = ResourceServer::GetInstance()->GetHandle("energy3");

	_drawX = DRAW_X + DRAW_OFFSET_X;
	_drawY = DRAW_Y + DRAW_OFFSET_Y;
}

EnergyUI::~EnergyUI()
{
	DeleteGraph(_iEnergyFrameHandle);
	DeleteGraph(_iEnergyHandle1);
	DeleteGraph(_iEnergyHandle2);
	DeleteGraph(_iEnergyHandle3);
}

bool EnergyUI::Initialize()
{
	return true;
}

bool EnergyUI::Terminate()
{
	return true;
}

bool EnergyUI::Process()
{
	return true;
}

bool EnergyUI::Render()
{
	// エネルギーフレーム表示関数
	EnergyFrameRender();

	// エネルギーゲージ表示関数
	GaugeRatioCalculation();

	return true;
}

// エネルギーフレーム表示関数
void EnergyUI::EnergyFrameRender()
{
	DrawGraph(DRAW_X, DRAW_Y, _iEnergyFrameHandle, TRUE);
}

// ゲージ表示比率計算関数
void EnergyUI::GaugeRatioCalculation()
{
	// エネルギーマネージャーから現在のエネルギー量を取得
	float currentEnergy = EnergyManager::GetInstance()->GetCurrentEnergy();
	float maxEnergy = EnergyManager::GetInstance()->GetMaxEnergy();

	if(maxEnergy <= 0.0f) return;  // 最大エネルギーが0以下の場合は描画しない

	// エネルギーの比率を計算
	float energyRatio = currentEnergy / maxEnergy;
	if(energyRatio > 1.0f){ energyRatio = 1.0f; }
	if(energyRatio < 0.0f){ energyRatio = 0.0f; }

	// エネルギーゲージ表示関数
	EnergyGaugeRender(energyRatio);
}

// エネルギーゲージ表示関数
void EnergyUI::EnergyGaugeRender(float ratio)
{
	if(ratio <= 0.0f){ return; }	// エネルギーが0以下の場合は描画しない

	// 元画像のサイズを取得
	int graphW, graphH;
	GetGraphSize(_iEnergyHandle1, &graphW, &graphH);

	// 1段目のゲージ
	if(ratio > 0.0f && ratio <= 0.333f)
	{
		// クリッピング領域を設定
		float segmentRatio = ratio / 0.333f;						// 0.0f～1.0fの範囲に変換
		int clipHeight = static_cast<int>(graphH * segmentRatio);	// 描画する高さ
		int clipY = _drawY + (graphH - clipHeight);					// 下から満たすためのY座標

		// クリッピング領域を設定
		SetDrawArea(_drawX, clipY, _drawX + graphW, _drawY + graphH);
		DrawGraph(_drawX, _drawY, _iEnergyHandle1, TRUE);
	}
	// 2段目のゲージ
	else if(ratio > 0.333f && ratio <= 0.666f)
	{
		// 描画領域を元に戻す
		SetDrawAreaFull();	

		// 1段階目は完全表示
		DrawGraph(_drawX, _drawY, _iEnergyHandle1, TRUE);

		// 2段目のゲージ
		float segmentRatio = (ratio - 0.333f) / 0.333f;						// 0.0f～1.0fの範囲に変換
		int segmentClipHeight = static_cast<int>(graphH * segmentRatio);	// 描画する高さ
		int segmentClipY = _drawY + (graphH - segmentClipHeight);			// 下から満たすためのY座標

		// クリッピング領域を設定
		SetDrawArea(_drawX, segmentClipY, _drawX + graphW, _drawY + graphH);
		DrawGraph(_drawX, _drawY, _iEnergyHandle2, TRUE);
	}
	// 3段目のゲージ
	else if(ratio > 0.666f)
	{
		// 描画領域を元に戻す
		SetDrawAreaFull();

		// 1、2段階目は完全表示
		DrawGraph(_drawX, _drawY, _iEnergyHandle1, TRUE);
		DrawGraph(_drawX, _drawY, _iEnergyHandle2, TRUE);

		// 3段目のゲージ
		float segmentRatio = (ratio - 0.666f) / 0.334f;						// 0.0f～1.0fの範囲に変換
		int segmentClipHeight = static_cast<int>(graphH * segmentRatio);	// 描画する高さ
		int segmentClipY = _drawY + (graphH - segmentClipHeight);			// 下から満たすためのY座標

		// クリッピング領域を設定
		SetDrawArea(_drawX, segmentClipY, _drawX + graphW, _drawY + graphH);
		DrawGraph(_drawX, _drawY, _iEnergyHandle3, TRUE);
	}

	// 描画領域を元に戻す
	SetDrawAreaFull();
}