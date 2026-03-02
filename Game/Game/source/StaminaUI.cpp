#include "StaminaUI.h"
#include "StaminaManager.h"

namespace
{
	// 画像表示用
	const int DRAW_BAR_FRAME_X = 216;
	const int DRAW_BAR_FRAME_Y = 1000;
	const int DRAW_OFFSET_X = 15;
	const int DRAW_OFFSET_Y = 5;
}

StaminaUI::StaminaUI()
{
	_iStaminaFrameHandle = ResourceServer::GetInstance()->GetHandle("StaminaFrame");
	_iStaminaHandle = ResourceServer::GetInstance()->GetHandle("Stamina");

	_drawStaminaBarX = DRAW_BAR_FRAME_X + DRAW_OFFSET_X;
	_drawStaminaBarY = DRAW_BAR_FRAME_Y + DRAW_OFFSET_Y;
	_drawStaminaBarFrameX = DRAW_BAR_FRAME_X;
	_drawStaminaBarFrameY = DRAW_BAR_FRAME_Y;
}

StaminaUI::~StaminaUI()
{
	DeleteGraph(_iStaminaFrameHandle);
	DeleteGraph(_iStaminaHandle);
}

bool StaminaUI::Initialize()
{
	return true;
}

bool StaminaUI::Terminate()
{
	return true;
}

bool StaminaUI::Process()
{
	return true;
}

bool StaminaUI::Render()
{
	// Zバッファ無効化
	SetUseZBuffer3D(FALSE);

	// スタミナフレーム表示関数
	//StaminaFrameRender();

	// スタミナゲージ表示関数
	//GaugeRatioCalculation();

	// Zバッファ有効化
	SetUseZBuffer3D(TRUE);

	return true;
}

// スタミナフレーム表示関数
void StaminaUI::StaminaFrameRender()
{
	DrawGraph(_drawStaminaBarFrameX, _drawStaminaBarFrameY, _iStaminaFrameHandle, TRUE);
}

// ゲージ表示比率計算関数
void StaminaUI::GaugeRatioCalculation()
{
	// スタミナマネージャーから現在のスタミナ量を取得
	float currentStamina = StaminaManager::GetInstance()->GetCurrentStamina();
	float maxStamina = StaminaManager::GetInstance()->GetMaxStamina();

	if(maxStamina <= 0.0f) return;  // 最大スタミナが0以下の場合は描画しない

	// スタミナの比率を計算
	float staminaRatio = currentStamina / maxStamina;
	if(staminaRatio > 1.0f){ staminaRatio = 1.0f; }
	if(staminaRatio < 0.0f){ staminaRatio = 0.0f; }

	// スタミナゲージ表示関数
	StaminaGaugeRender(staminaRatio);
}

// スタミナゲージ表示関数
void StaminaUI::StaminaGaugeRender(float ratio)
{
	if(ratio <= 0.0f){ return; }	// 比率が0以下の場合は描画しない

	// 画像サイズ
	int graphW, graphH;
	GetGraphSize(_iStaminaHandle, &graphW, &graphH);

	// 描画する幅
	int clipW = static_cast<int>(graphW * ratio);

	// クリッピング領域を設定
	SetDrawArea(_drawStaminaBarX, _drawStaminaBarY, _drawStaminaBarX + clipW, _drawStaminaBarY + graphH);

	// スタミナゲージ描画
	DrawGraph(_drawStaminaBarX, _drawStaminaBarY, _iStaminaHandle, TRUE);

	// クリッピング領域を元に戻す
	SetDrawAreaFull();
}
