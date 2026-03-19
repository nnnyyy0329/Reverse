#include "ModeEndingText.h"
#include "ModeLogo.h"
#include <algorithm>

bool ModeEndingText::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;

	_fadeOutStarted = false;
	_textStarted = false;

	_charIndex = 0;
	_charTimer = 0;
	_textFullyShown = false;

	_text =
		"｢結局白雪は旧校舎（?）の奥で気を失っていた。\n"
		"意識を取り戻した後に話を聞いたが、\nどうやらあの日の記憶が曖昧みたいで、うやむやになってしまった。\n"
		"だが、あの日は俺とクロがこれから巻き込まれることのきっかけだったとあとから気づくのだった。";

	// まずフェードアウトして真っ黒へ
	StartFade(30, 0, 30); // out=30 / in=0 / wait=30（ModeBaseの実装に依存するが、ここは真っ黒にする目的）
	_fadeOutStarted = true;

	return true;
}

bool ModeEndingText::Terminate()
{
	base::Terminate();

	ResourceServer::GetInstance()->Terminate();  // リソースサーバーの終了

	return true;
}

bool ModeEndingText::Process()
{
	base::Process();

	_frameCount++;

	AdvanceFade();
	if(IsFadeActive() && GetFadeAlpha() <= 0)
	{
		StopFade();
		_textStarted = true;
	}

	// フェードが終わるまでテキスト開始しない
	if(!_textStarted)
	{
		return true;
	}

	// サウンド再生
	SoundServer::GetInstance()->Play("BGM_Ending", DX_PLAYTYPE_LOOP);

	// 文字自動展開
	if(!_textFullyShown)
	{
		_charTimer++;
		if(_charTimer >= kCharInterval)
		{
			_charTimer = 0;
			if(_charIndex < static_cast<int>(_text.size()))
			{
				++_charIndex;
			}
			if(_charIndex >= static_cast<int>(_text.size()))
			{
				_textFullyShown = true;
			}
		}
	}

	// 入力
	auto& im = InputManager::GetInstance();
	const bool canInput = (_frameCount >= kInputWaitFrames);

	if(canInput && ( im.IsTrigger(INPUT_ACTION::ATTACK)))
	{
		if(!_textFullyShown)
		{
			_charIndex = static_cast<int>(_text.size());
			_textFullyShown = true;
		}
		else
		{
			// サウンド停止
			SoundServer::GetInstance()->Stop("BGM_Ending");

			// もう一回確定でLOGOへ
			ModeServer::GetInstance()->Clear();
			ModeServer::GetInstance()->Add(new ModeLogo(), 100, "logo");
			return true;
		}
	}

	return true;
}

bool ModeEndingText::Render()
{
	ClearDrawScreen();

	// 常に黒背景
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);

	// フェード中は黒のまま（alpha次第で上書きしてもよいが、黒固定でOK）
	// テキスト
	if(_textStarted)
	{
		const int showCount = std::max(0, std::min(_charIndex, static_cast<int>(_text.size())));
		const std::string display = _text.substr(0, showCount);

		SetFontSize(28);
		DrawFormatString(120, 780, GetColor(255, 255, 255), "%s", display.c_str());
		SetFontSize(16);
	}

	return true;
}