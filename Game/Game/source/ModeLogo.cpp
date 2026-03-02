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

	int ClampAlpha(int a)
	{
		if(a < 0) { return 0; }
		if(a > 255) { return 255; }
		return a;
	}

	int CalcAlpha(int frame)
	{
		if(frame < FADE_IN_FRAMES)
		{
			const float t = static_cast<float>(frame) / static_cast<float>(FADE_IN_FRAMES);
			return ClampAlpha(static_cast<int>(255.0f * t));
		}

		frame -= FADE_IN_FRAMES;
		if(frame < HOLD_FRAMES)
		{
			return 255;
		}

		frame -= HOLD_FRAMES;
		if(frame < FADE_OUT_FRAMES)
		{
			const float t = static_cast<float>(frame) / static_cast<float>(FADE_OUT_FRAMES);
			return ClampAlpha(static_cast<int>(255.0f * (1.0f - t)));
		}

		return 0;
	}
}

bool ModeLogo::Initialize()
{
	if(!base::Initialize()) { return false; }

	_logoHandle = LoadGraph("res/Graph/AMGlogo.png");
	_frameCount = 0;
	_bNext = false;

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

	_frameCount++;
	if(_frameCount >= TOTAL_FRAMES)
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

		const int a = CalcAlpha(_frameCount);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DrawExtendGraph(x, y, x + scaledW, y + scaledH, _logoHandle, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	return true;
}