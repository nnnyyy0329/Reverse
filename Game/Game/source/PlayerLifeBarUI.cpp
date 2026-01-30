#include "PlayerLifeBarUI.h"
#include "PlayerManager.h"

namespace
{
	// 画像表示用
	const int DRAW_BAR_FRAME_X = 216;
	const int DRAW_BAR_FRAME_Y = 1000;
	const int DRAW_OFFSET_X = 6;
	const int DRAW_OFFSET_Y = 6;
}

PlayerLifeBarUI::PlayerLifeBarUI()
{
	_iLifeBar = ResourceServer::GetInstance()->GetHandle("PlayerLifeBar");
	_iLifeBarFrame = ResourceServer::GetInstance()->GetHandle("PlayerLifeBarFrame");

	_drawLifeBarX = DRAW_BAR_FRAME_X + DRAW_OFFSET_X;	// ライフバーの描画位置X
	_drawLifeBarY = DRAW_BAR_FRAME_Y + DRAW_OFFSET_Y;	// ライフバーの描画位置Y
	_drawLifeBarFrameX = DRAW_BAR_FRAME_X;				// ライフバーフレームの描画位置X
	_drawLifeBarFrameY = DRAW_BAR_FRAME_Y;				// ライフバーフレームの描画位置Y

	_playerManager = nullptr;
}

PlayerLifeBarUI::~PlayerLifeBarUI()
{
	DeleteGraph(_iLifeBar);
	DeleteGraph(_iLifeBarFrame);
}

bool PlayerLifeBarUI::Initialize()
{
	return true;
}

bool PlayerLifeBarUI::Terminate()
{
	return true;
}

bool PlayerLifeBarUI::Process()
{
	return true;
}

bool PlayerLifeBarUI::Render()
{
	// Zバッファ無効化
	SetUseZBuffer3D(FALSE);

	// ライフバーフレーム描画
	LifeBarRenderFrame();

	// ライフバー比率計算
	BarRatioCalculation();

	// Zバッファ有効化
	SetUseZBuffer3D(TRUE);

	return true;
}

// ライフバーフレーム描画
void PlayerLifeBarUI::LifeBarRenderFrame()
{
	DrawGraph(_drawLifeBarFrameX, _drawLifeBarFrameY, _iLifeBarFrame, TRUE);
}

// ライフバー比率計算
void PlayerLifeBarUI::BarRatioCalculation()
{
	if(_playerManager == nullptr){ return; }	

	// プレイヤーの体力を取得
	float currentLife = _playerManager->GetActivePlayerShared()->GetLife();
	float maxLife = _playerManager->GetActivePlayerShared()->GetMaxLife();

	if(maxLife <= 0.0f){ return; }	// 最大体力が0以下の場合は描画しない

	// 体力の比率を計算
	float lifeRatio = currentLife / maxLife;
	if(lifeRatio > 1.0f){ lifeRatio = 1.0f; }	// 上限チェック
	if(lifeRatio < 0.0f){ lifeRatio = 0.0f; }	// 下限チェック

	// ライフバーゲージ表示関数
	LifeBarRender(lifeRatio);
}

// ライフバー描画
void PlayerLifeBarUI::LifeBarRender(float ratio)
{
	if(ratio <= 0.0f){ return; }	// 比率が0以下の場合は描画しない

	// 画像サイズ
	int graphW, graphH;
	GetGraphSize(_iLifeBar, &graphW, &graphH);

	// 描画する幅
	int clipW = static_cast<int>(graphW * ratio);

	// クリッピング領域を設定
	SetDrawArea(_drawLifeBarX, _drawLifeBarY, _drawLifeBarX + clipW, _drawLifeBarY + graphH);

	// ライフバー描画
	DrawGraph(_drawLifeBarX, _drawLifeBarY, _iLifeBar, TRUE);

	// クリッピング領域を元に戻す
	SetDrawAreaFull();
}
