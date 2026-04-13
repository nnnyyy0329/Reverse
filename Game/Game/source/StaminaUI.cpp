#include "StaminaUI.h"
#include "StaminaManager.h"

// スタミナUI表示設定用定数のエイリアス
namespace SRC = StaminaRenderConfig;

StaminaUI::StaminaUI()
{
	_iStaminaFrameHandle = ResourceServer::GetInstance()->GetHandle("StaminaFrame");
	_iStaminaHandle = ResourceServer::GetInstance()->GetHandle("Stamina");

	_drawStaminaBarX = SRC::DRAW_BAR_FRAME_X + SRC::DRAW_OFFSET_X;
	_drawStaminaBarY = SRC::DRAW_BAR_FRAME_Y + SRC::DRAW_OFFSET_Y;
	_drawStaminaBarFrameX = SRC::DRAW_BAR_FRAME_X;
	_drawStaminaBarFrameY = SRC::DRAW_BAR_FRAME_Y;
}

StaminaUI::~StaminaUI()
{
	// デストラクタ
	Terminate();
}

bool StaminaUI::Initialize()
{
	return true;
}

bool StaminaUI::Terminate()
{
	// 画像の開放
	DeleteGraph(_iStaminaFrameHandle);
	DeleteGraph(_iStaminaHandle);

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

void StaminaUI::StaminaFrameRender()
{
	DrawGraph(_drawStaminaBarFrameX, _drawStaminaBarFrameY, _iStaminaFrameHandle, TRUE);
}

void StaminaUI::GaugeRatioCalculation()
{
	// スタミナマネージャーから現在のスタミナ量を取得
	float currentStamina = StaminaManager::GetInstance()->GetCurrentStamina();
	float maxStamina = StaminaManager::GetInstance()->GetMaxStamina();

	// 最大スタミナが0以下の場合は描画しない
	if(maxStamina <= 0.0f) return;  

	// スタミナの比率を計算
	float staminaRatio = currentStamina / maxStamina;
	if(staminaRatio > 1.0f){ staminaRatio = 1.0f; }
	if(staminaRatio < 0.0f){ staminaRatio = 0.0f; }

	// スタミナゲージ表示関数
	StaminaGaugeRender(staminaRatio);
}

void StaminaUI::StaminaGaugeRender(float ratio)
{
	// 比率が0以下の場合は描画しない
	if(ratio <= 0.0f){ return; }	

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
