#include "EnergyUI.h"
#include "EnergyManager.h"

namespace
{
	// 画像表示用
	const int DRAW_X = 10;
	const int DRAW_Y = 10;
	const int DRAAW_OFFSET = 20;

	// 円形ゲージ用
	const int CIRCLE_CENTER_X = 90;  // 円の中心X座標
	const int CIRCLE_CENTER_Y = 90;  // 円の中心Y座標
	const int CIRCLE_RADIUS = 90;    // 円の半径
}

EnergyUI::EnergyUI()
{
	_iEnergyFrameHandle = ResourceServer::GetInstance()->GetHandle("energyFrame");
	_iEnergyHandle1 = ResourceServer::GetInstance()->GetHandle("tryEnergy1");
	//_iEnergyHandle1 = ResourceServer::GetInstance()->GetHandle("energy1");
	_iEnergyHandle2 = ResourceServer::GetInstance()->GetHandle("energy2");
	_iEnergyHandle3 = ResourceServer::GetInstance()->GetHandle("energy3");
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
	//DrawGraph(DRAW_X, DRAW_Y, _iEnergyFrameHandle, TRUE);
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

	// 円の下端からfillHeightの高さまでを描画
	int cutY = static_cast<int>(graphH * (1.0f - ratio));	// 上部をカット
	int cutH = static_cast<int>(graphH * ratio);			// 描画する高さ

	// 下から満たされるように画面上での描画Y座標を調整
	int fillScreenY = DRAW_Y + cutY;	// 上部がカットされた分だけY座標を下げる
	int fillDrawH = cutH;				// 描画する高さ

	// 円形ゲージを描画（下から上に満たされる）
	if(cutH > 0)
	{
		DrawRectExtendGraph
		(
			DRAW_X, fillScreenY,						// 描画開始位置
			DRAW_X + graphW, fillScreenY + fillDrawH,	// 描画終了位置
			0, cutY,									// 元画像の切り取り開始位置
			graphW, cutY + cutH,						// 元画像の切り取り終了位置
			_iEnergyHandle1,							// 画像ハンドル
			TRUE										// 透明度有効
		);
	}

	//DrawGraph(DRAW_X, DRAW_Y, _iEnergyHandle1, TRUE);

}