#include "ModeTitle.h"
#include "ModeLoading.h"
#include "ApplicationMain.h"

#include <cstdlib>

namespace
{
	constexpr int MENU_COUNT = 3;

	void DrawMenuItem(int x, int y, const char* text, bool selected)
	{
		SetFontSize(48);

		const int glowColor = selected ? GetColor(255, 255, 255) : GetColor(160, 0, 255);
		const int bodyColor = selected ? GetColor(255, 255, 255) : GetColor(130, 0, 130);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80);
		for(int dy = -2; dy <= 2; ++dy)
		{
			for(int dx = -2; dx <= 2; ++dx)
			{
				if(dx == 0 && dy == 0) { continue; }
				DrawFormatString(x + dx, y + dy, glowColor, text);
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawFormatString(x, y, bodyColor, text);

		SetFontSize(16);
	}

	void ExitGame()
	{
		Effkseer_End();
		DxLib_End();
	}

	int WrapMenuIndex(int index)
	{
		if(index < 0) { return MENU_COUNT - 1; }
		if(index >= MENU_COUNT) { return 0; }
		return index;
	}
}

bool ModeTitle::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddLoading = false;

	_titleHandle = LoadGraph("res/Graph/Title.png");
	_alpha = 0;
	_fadeState = 0; // 0:フェードイン, 1:表示, 2:フェードアウト
	_frameCount = 0;

	_menuIndex = 0;

	return true;
}

bool ModeTitle::Terminate() 
{
	base::Terminate();
	return true;
}

bool ModeTitle::Process() 
{
	const int trg = ApplicationMain::GetInstance()->GetTrg();

	// ↑↓で選択
	if(trg & PAD_INPUT_UP)
	{
		_menuIndex = WrapMenuIndex(_menuIndex - 1);
	}
	if(trg & PAD_INPUT_DOWN)
	{
		_menuIndex = WrapMenuIndex(_menuIndex + 1);
	}

	// 決定
	if(trg & PAD_INPUT_10)
	{
		switch(_menuIndex)
		{
			case 0: // スタート
			{
				if(!_bIsAddLoading)
				{
					_bIsAddLoading = true;
					ModeServer::GetInstance()->Add(new ModeLoading(), 9999, "loading");
					ModeServer::GetInstance()->Del(this);
				}
				break;
			}
			case 1: // オプション(仮)
			{
				// TODO: 後で ModeOption 等に差し替え
				break;
			}
			case 2: // ゲーム終了
			{
				ExitGame();
				break;
			}
		}
	}

	return true;
}

bool ModeTitle::Render()
{
	ClearDrawScreen();

	// タイトル画像
	{
		int w, h;
		GetGraphSize(_titleHandle, &w, &h);

		float scale = 0.5f;
		int scaledW = static_cast<int>(w * scale);
		int scaledH = static_cast<int>(h * scale);

		int x = (1920 - scaledW) / 2;
		int y = (1080 - scaledH) / 2;

		DrawExtendGraph(x, y, x + scaledW, y + scaledH, _titleHandle, TRUE);
	}

	// メニュー
	{
		const int textX = 840;
		const int textY = 820;
		const int lineGap = 70;

		DrawMenuItem(textX, textY + 0 * lineGap, "スタート", _menuIndex == 0);
		DrawMenuItem(textX, textY + 1 * lineGap, "オプション", _menuIndex == 1);
		DrawMenuItem(textX, textY + 2 * lineGap, "ゲーム終了", _menuIndex == 2);
	}

	return true;
}