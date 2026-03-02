#include "ModeGameOver.h"
#include "ModeGame.h"
#include "ApplicationMain.h"

#include <cstdlib>

namespace
{
	constexpr int MENU_COUNT = 2;

	void DrawMenuItem(int x, int y, const char* text, bool selected)
	{
		SetFontSize(48);

		const int glowColor = selected ? GetColor(255, 255, 255) : GetColor(255, 0, 0);
		const int bodyColor = selected ? GetColor(255, 255, 255) : GetColor(200, 200, 200);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
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

	void DrawSelectArrow(int x, int y, int size, unsigned int color)
	{
		const int x0 = x;
		const int y0 = y;
		const int x1 = x;
		const int y1 = y + size;
		const int x2 = x + size;
		const int y2 = y + (size / 2);

		DrawTriangle(x0, y0, x1, y1, x2, y2, color, TRUE);
	}

	void ExitGame()
	{
		Effkseer_End();
		DxLib_End();
		std::exit(0);
	}

	int WrapMenuIndex(int index)
	{
		if(index < 0) { return MENU_COUNT - 1; }
		if(index >= MENU_COUNT) { return 0; }
		return index;
	}
}

ModeGameOver::ModeGameOver()
{
}

ModeGameOver::~ModeGameOver()
{
}

bool ModeGameOver::Initialize()
{
	if(!base::Initialize()) { return false; }
	_menuIndex = 0;
	return true;
}

bool ModeGameOver::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeGameOver::Process()
{
	base::Process();

	// これより下のモードの処理をスキップ
	ModeServer::GetInstance()->SkipProcessUnderLayer();

	//int trg = ApplicationMain::GetInstance()->GetTrg();

	//if (trg & PAD_INPUT_10) {// spaceキー
	//	// 現在のステージ番号からリスタート
	//	ModeGame* modeGame = (ModeGame*)ModeServer::GetInstance()->Get("game");
	//	if (modeGame != nullptr)
	//	{
	//		modeGame->RestartCurrentStage();
	//	}

	//	// このモードを削除
	//	ModeServer::GetInstance()->Del(this);
	//}

	return true;
}

bool ModeGameOver::Render()
{
	base::Render();

	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


	SetFontSize(48);
	DrawFormatString(720, 260, GetColor(255, 0, 0), "===== お前の負けｗ =====");
	SetFontSize(16);
	// メニュー
	{
		const int textX = 760;
		const int textY = 420;
		const int lineGap = 70;

		DrawMenuItem(textX, textY + 0 * lineGap, "ゲームリトライ", _menuIndex == 0);
		DrawMenuItem(textX, textY + 1 * lineGap, "ゲーム終了", _menuIndex == 1);

		const int arrowX = textX - 50;
		const int arrowY = textY + _menuIndex * lineGap + 10;
		DrawSelectArrow(arrowX, arrowY, 28, GetColor(255, 255, 255));
	}

	return true;
}