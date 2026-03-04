#include "ModeScenario.h"
#include "ModeGame.h"

bool ModeScenario::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;
	_pageIndex = 0;
	_bNext = false;

	_textHandles[0] = ResourceServer::GetInstance()->GetHandle("GameStartText");
	_textHandles[1] = ResourceServer::GetInstance()->GetHandle("GameStartText1");

	return true;
}

bool ModeScenario::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeScenario::Process()
{
	_frameCount++;

	auto input = InputManager::GetInstance();

	// 連打で一気に飛ばないように、数フレーム待つ（任意）
	const int kInputWaitFrames = 10;
	const bool canInput = (_frameCount >= kInputWaitFrames);

	if(canInput && (input->IsTrigger(INPUT_ACTION::SELECT) || input->IsTrigger(INPUT_ACTION::ATTACK)))
	{
		_bNext = true;
	}

	if(_bNext)
	{
		_bNext = false;
		_frameCount = 0;
		_pageIndex++;

		// 2枚終わったらゲームへ
		if(_pageIndex >= 2)
		{
			ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
			ModeServer::GetInstance()->Del(this);
		}
	}

	return true;
}

bool ModeScenario::Render()
{
	ClearDrawScreen();

	const int handle = (_pageIndex >= 0 && _pageIndex < 2) ? _textHandles[_pageIndex] : -1;
	if(handle >= 0)
	{
		int w = 0;
		int h = 0;
		GetGraphSize(handle, &w, &h);

		const int x = (1920 - w) / 2;
		const int y = (1080 - h) / 2;

		DrawGraph(x, y, handle, TRUE);
	}

	// 操作ガイド（任意）
	SetFontSize(24);
	DrawFormatString(60, 980, GetColor(255, 255, 255), "決定/攻撃で次へ");
	SetFontSize(16);

	return true;
}