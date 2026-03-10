#include "ModeLogo.h"
#include "ModeGame.h"
#include "ModeTitle.h"
#include "ApplicationMain.h"

namespace
{
	constexpr int FADE_IN_FRAMES = 30;
	constexpr int HOLD_FRAMES = 90;
	constexpr int FADE_OUT_FRAMES = 30;
	constexpr int TOTAL_FRAMES = FADE_IN_FRAMES + HOLD_FRAMES + FADE_OUT_FRAMES;
}

bool ModeLogo::Initialize()
{
	if(!base::Initialize()) { return false; }

	_logoHandle = LoadGraph("res/Graph/AMGlogo.png");
	_frameCount = 0;
	_bNext = false;

	// ModeBase の共通フェードを使う
	StartFade(FADE_IN_FRAMES, HOLD_FRAMES, FADE_OUT_FRAMES);

	return true;
}

bool ModeLogo::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeLogo::Process()
{
	auto inputManager = InputManager::GetInstance();

	// Aボタンを押したらスキップ
	if(inputManager->IsTrigger(INPUT_ACTION::SELECT))
	{
		_bNext = true;
	}

	// フェードの内部カウンタを進める
	AdvanceFade();

	_frameCount++;
	// フェードが終了したら次へ
	if(IsFadeFinished())
	{
		_bNext = true;
	}

	if(_bNext)
	{
		ModeServer::GetInstance()->Add(new ModeTitle(), 100, "title");
		ModeServer::GetInstance()->Del(this);
	}

	return true;
}

bool ModeLogo::Render()
{
	// WinMain側でも ClearDrawScreen() しているが、Mode単体でも完結させるなら残してOK
	ClearDrawScreen();

	if(_logoHandle >= 0)
	{
		int w = 0;
		int h = 0;
		GetGraphSize(_logoHandle, &w, &h);

		const float scale = 1.0f;
		const int scaledW = static_cast<int>(w * scale);
		const int scaledH = static_cast<int>(h * scale);

		const int x = (1920 - scaledW) / 2;
		const int y = (1080 - scaledH) / 2;

		const int a = GetFadeAlpha();

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DrawExtendGraph(x, y, x + scaledW, y + scaledH, _logoHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	return true;
}