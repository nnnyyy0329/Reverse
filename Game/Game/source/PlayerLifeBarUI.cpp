#include "PlayerLifeBarUI.h"
#include "PlayerManager.h"

namespace
{
	// 画像表示用
	constexpr int DRAW_BAR_FRAME_X = 216;
	constexpr int DRAW_BAR_FRAME_Y = 1000;
	constexpr int DRAW_OFFSET_X = 1;
	constexpr int DRAW_OFFSET_Y = 1;

	// 追加：縦方向スケール
	constexpr float LIFE_BAR_SCALE_Y = 0.2f;
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
	// 画像の解放
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
	// ライフバーのフレーム描画
	int w = 0;
	int h = 0;
	GetGraphSize(_iLifeBarFrame, &w, &h);

	const int drawH = static_cast<int>(h * LIFE_BAR_SCALE_Y);

	DrawExtendGraph(
		_drawLifeBarFrameX,
		_drawLifeBarFrameY,
		_drawLifeBarFrameX + w,
		_drawLifeBarFrameY + drawH,
		_iLifeBarFrame,
		TRUE
	);
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
	if(ratio <= 0.0f) { return; }	// 比率が0以下の場合は描画しない

	// 画像サイズ
	int graphW, graphH;
	GetGraphSize(_iLifeBar, &graphW, &graphH);

	// 描画する幅
	const int drawH = static_cast<int>(graphH * LIFE_BAR_SCALE_Y);
	const int clipW = static_cast<int>(graphW * ratio);

	// バー矩形（実際の表示高さは drawH）
	const int x0 = _drawLifeBarX;
	const int y0 = _drawLifeBarY;
	const int x1 = _drawLifeBarX + graphW;
	const int y1 = _drawLifeBarY + drawH;

	// 1) 影（ドロップシャドウ）: 少し右下にズラして暗く描く
	{
		const int shadowOffsetX = 2;
		const int shadowOffsetY = 2;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
		SetDrawArea(x0 + shadowOffsetX, y0 + shadowOffsetY, x0 + shadowOffsetX + clipW, y0 + shadowOffsetY + drawH);
		DrawExtendGraph(
			x0 + shadowOffsetX,
			y0 + shadowOffsetY,
			x1 + shadowOffsetX,
			y1 + shadowOffsetY,
			_iLifeBar,
			TRUE
		);
		SetDrawAreaFull();
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 2) 本体（従来どおりクリップして描画）
	{
		SetDrawArea(x0, y0, x0 + clipW, y0 + drawH);
		DrawExtendGraph(x0, y0, x1, y1, _iLifeBar, TRUE);
		SetDrawAreaFull();
	}

	// 3) 上面ハイライト（細い白帯）
	{
		const int highlightH = 3;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
		DrawBox(x0, y0, x0 + clipW, y0 + highlightH, GetColor(255, 255, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 4) 下面の影（細い黒帯）
	{
		const int shadeH = 3;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
		DrawBox(x0, y1 - shadeH, x0 + clipW, y1, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 5)（任意）減った部分を暗くして「溝」っぽくする
	if(clipW < graphW)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
		DrawBox(x0 + clipW, y0, x1, y1, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}
