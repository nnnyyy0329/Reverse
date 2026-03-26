#include "PlayerLifeBarUI.h"
#include "PlayerManager.h"

// 体力バー表示設定定数のエイリアス
namespace PLBC = PlayerLifeBarConfig;

PlayerLifeBarUI::PlayerLifeBarUI()
{
	_iLifeBar = ResourceServer::GetInstance()->GetHandle("PlayerLifeBar");
	_iLifeBarFrame = ResourceServer::GetInstance()->GetHandle("PlayerLifeBarFrame");

	_drawLifeBarX = PLBC::DRAW_BAR_FRAME_X + PLBC::DRAW_OFFSET_X;	// ライフバーの描画位置X
	_drawLifeBarY = PLBC::DRAW_BAR_FRAME_Y + PLBC::DRAW_OFFSET_Y;	// ライフバーの描画位置Y
	_drawLifeBarFrameX = PLBC::DRAW_BAR_FRAME_X;					// ライフバーフレームの描画位置X
	_drawLifeBarFrameY = PLBC::DRAW_BAR_FRAME_Y;					// ライフバーフレームの描画位置Y

	_flashTimer = 0.0f;		// 点滅タイマー
	_flashDuration = 0.0f;	// 点滅継続時間
	_isFlashing = false;	// 点滅中フラグ

	_playerManager = nullptr;
}

PlayerLifeBarUI::~PlayerLifeBarUI()
{
	// デストラクタ
	Terminate();
}

bool PlayerLifeBarUI::Initialize()
{
	return true;
}

bool PlayerLifeBarUI::Terminate()
{
	// 画像の解放
	DeleteGraph(_iLifeBar);
	DeleteGraph(_iLifeBarFrame);

	return true;
}

bool PlayerLifeBarUI::Process()
{
	// 点滅エフェクトの更新
	UpdateFlashEffect();

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

	//// ライフバーのフレーム描画
	//int w = 0;
	//int h = 0;
	//GetGraphSize(_iLifeBarFrame, &w, &h);

	//const int drawH = static_cast<int>(h * LIFE_BAR_SCALE_Y);

	//DrawExtendGraph(
	//	_drawLifeBarFrameX,
	//	_drawLifeBarFrameY,
	//	_drawLifeBarFrameX + w,
	//	_drawLifeBarFrameY + drawH,
	//	_iLifeBarFrame,
	//	TRUE
	//);
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

	// 上限チェック
	if(lifeRatio > 1.0f){ lifeRatio = 1.0f; }	

	// 下限チェック
	if(lifeRatio < 0.0f){ lifeRatio = 0.0f; }	

	// ライフバーゲージ表示関数
	LifeBarRender(lifeRatio);
}

// ライフバー描画
void PlayerLifeBarUI::LifeBarRender(float ratio)
{
	// 体力の比率が0以下の場合は描画しない
	if(ratio <= 0.0f) { return; }

	// 画像サイズを取得
	int graphW, graphH;
	GetGraphSize(_iLifeBar, &graphW, &graphH);

	// 描画する幅を計算
	int clipW = static_cast<int>(graphW * ratio);

	// バー矩形を設定
	SetDrawArea(_drawLifeBarX, _drawLifeBarY, _drawLifeBarX + clipW, _drawLifeBarY + graphH);

	// ライフバーを描画
	DrawGraph(_drawLifeBarX, _drawLifeBarY, _iLifeBar, TRUE);

	// 描画領域を全体に戻す
	SetDrawAreaFull();

	// 点滅エフェクトの描画
	FlashEffectRender(clipW, graphH);


	//if(ratio <= 0.0f) { return; }	// 比率が0以下の場合は描画しない

	//// 画像サイズ
	//int graphW, graphH;
	//GetGraphSize(_iLifeBar, &graphW, &graphH);

	//// 描画する幅
	//const int drawH = static_cast<int>(graphH * LIFE_BAR_SCALE_Y);
	//const int clipW = static_cast<int>(graphW * ratio);

	//// バー矩形（実際の表示高さは drawH）
	//const int x0 = _drawLifeBarX;
	//const int y0 = _drawLifeBarY;
	//const int x1 = _drawLifeBarX + graphW;
	//const int y1 = _drawLifeBarY + drawH;

	//// 1) 影（ドロップシャドウ）: 少し右下にズラして暗く描く
	//{
	//	const int shadowOffsetX = 2;
	//	const int shadowOffsetY = 2;

	//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
	//	SetDrawArea(x0 + shadowOffsetX, y0 + shadowOffsetY, x0 + shadowOffsetX + clipW, y0 + shadowOffsetY + drawH);
	//	DrawExtendGraph(
	//		x0 + shadowOffsetX,
	//		y0 + shadowOffsetY,
	//		x1 + shadowOffsetX,
	//		y1 + shadowOffsetY,
	//		_iLifeBar,
	//		TRUE
	//	);
	//	SetDrawAreaFull();
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//}

	//// 2) 本体（従来どおりクリップして描画）
	//{
	//	SetDrawArea(x0, y0, x0 + clipW, y0 + drawH);
	//	DrawExtendGraph(x0, y0, x1, y1, _iLifeBar, TRUE);
	//	SetDrawAreaFull();
	//}

	//// 3) 上面ハイライト（細い白帯）
	//{
	//	const int highlightH = 3;
	//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
	//	DrawBox(x0, y0, x0 + clipW, y0 + highlightH, GetColor(255, 255, 255), TRUE);
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//}

	//// 4) 下面の影（細い黒帯）
	//{
	//	const int shadeH = 3;
	//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
	//	DrawBox(x0, y1 - shadeH, x0 + clipW, y1, GetColor(0, 0, 0), TRUE);
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//}

	//// 5)（任意）減った部分を暗くして「溝」っぽくする
	//if(clipW < graphW)
	//{
	//	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
	//	DrawBox(x0 + clipW, y0, x1, y1, GetColor(0, 0, 0), TRUE);
	//	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	//}
}

void PlayerLifeBarUI::StartFlashEffect(float duration)
{
	_isFlashing = true;
	_flashTimer = 0.0f;
	_flashDuration = duration;
}

void PlayerLifeBarUI::UpdateFlashEffect()
{
	// 点滅中なら更新
	if(_isFlashing)
	{
		// フレームタイム
		_flashTimer += 1.0f;	

		// 点滅タイマーが継続時間を超えたら点滅終了
		if(_flashTimer >= _flashDuration)
		{
			_isFlashing = false;
			_flashTimer = 0.0f;
		}
	}
}

void PlayerLifeBarUI::FlashEffectRender(int clipW, int graphH)
{
	// 点滅フラグが有効なら
	if(_isFlashing)
	{
		// サインカーブで0～1の値を生成
		float blendAlpha = fabsf(sinf(_flashTimer / _flashDuration * DX_PI_F)) * FlashEffectConfig::FLASH_MAX_ALPHA;

		// 点滅のアルファ値を設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(blendAlpha));

		// 点滅用の矩形を設定(ライフバーと同じ位置・サイズ)
		SetDrawArea(_drawLifeBarX, _drawLifeBarY, _drawLifeBarX + clipW, _drawLifeBarY + graphH);

		DrawBox(_drawLifeBarX, _drawLifeBarY, _drawLifeBarX + clipW, _drawLifeBarY + graphH, GetColor(255, 255, 255), TRUE);

		// 描画領域を全体に戻す
		SetDrawAreaFull();

		// ブレンドモードを元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}