#include "ModeGameOver.h"
#include "ModeGame.h"
#include "ApplicationMain.h"
#include "ModeTitle.h"
#include <cstdlib>

namespace
{
	constexpr int MENU_COUNT = 2;

	void DrawMenuImage(int centerX, int topY, int handle, bool selected)
	{
		if(handle < 0)
		{
			// ハンドルが無ければフォールバックで枠だけ描画
			const int w = 300;
			const int h = 64;
			const int x = centerX - w / 2;
			const int y = topY;
			const int bg = selected ? GetColor(255, 255, 255) : GetColor(100, 100, 100);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, selected ? 160 : 80);
			DrawBox(x - 8, y - 8, x + w + 8, y + h + 8, bg, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			return;
		}

		int w = 0, h = 0;
		GetGraphSize(handle, &w, &h);

		const int x = centerX - w / 2;
		const int y = topY;

		// 選択中は薄いライトを背後に描画して強調
		if(selected)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 140);
			const int glowCol = GetColor(255, 255, 255);
			DrawBox(x - 12, y - 12, x + w + 12, y + h + 12, glowCol, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}

		DrawGraph(x, y, handle, TRUE);
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

	auto& im = InputManager::GetInstance();

	// ↑↓で選択できるように追加
	if(im.IsTrigger(INPUT_ACTION::MOVE_UP))
	{
		_menuIndex = WrapMenuIndex(_menuIndex - 1);
	}
	if(im.IsTrigger(INPUT_ACTION::MOVE_DOWN))
	{
		_menuIndex = WrapMenuIndex(_menuIndex + 1);
	}

	// 決定（SKIP）で選択項目に応じた処理
	if(im.IsTrigger(INPUT_ACTION::SKIP))
	{
		if(_menuIndex == 0) // リトライ
		{
			ModeGame* modeGame = (ModeGame*)ModeServer::GetInstance()->Get("game");
			if(modeGame != nullptr)
			{
				modeGame->RestartCurrentStage();
			}
			ModeServer::GetInstance()->Del(this);
		}
		else if(_menuIndex == 1) // タイトルへ戻る
		{
			// BGM停止
			SoundServer::GetInstance()->StopAll();

			// 先にゲームモードを消しておく（タイトルが背後に表示されるのを防ぐ）
			auto gm = ModeServer::GetInstance()->Get("game");
			if(gm != nullptr)
			{
				ModeServer::GetInstance()->Del(gm);
			}

			// タイトルをトップに追加（優先度を大きめに）
			ModeServer::GetInstance()->Add(new ModeTitle(), 9999, "title");

			// 自分（GameOver）を削除
			ModeServer::GetInstance()->Del(this);
		}

		return true;
	}
}

bool ModeGameOver::Render()
{
	base::Render();

	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 画像ハンドル取得
	auto rs = ResourceServer::GetInstance();
	const int hGameOver = rs->GetHandle("GameOver");
	const int hRetry = rs->GetHandle("Retry");
	const int hTitle = rs->GetHandle("Title");

	// Game Over 画像（上部に中央寄せで表示）
	if(hGameOver >= 0)
	{
		int w = 0, h = 0;
		GetGraphSize(hGameOver, &w, &h);
		const int x = (1920 - w) / 2;
		const int y = 180;
		DrawGraph(x, y, hGameOver, TRUE);
	}


	// メニュー（画像で表示）
	{
		const int centerX = 960; // 中央
		const int startY = 460;
		const int lineGap = 140; // 画像の大きさに合わせて余裕を持たせる

		// Retry（メニュー 0）
		DrawMenuImage(centerX, startY + 0 * lineGap, hRetry, _menuIndex == 0);

		// Title（メニュー 1）
		DrawMenuImage(centerX, startY + 1 * lineGap, hTitle, _menuIndex == 1);

		// 矢印も表示（画像が無くても視認性を確保）
		const int arrowX = centerX - 300;
		const int arrowY = startY + _menuIndex * lineGap + 20;
		DrawSelectArrow(arrowX, arrowY, 28, GetColor(255, 255, 255));
	}

	return true;
}