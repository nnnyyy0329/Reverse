#include "ModeEndingText.h"
#include "ModeLogo.h"
#include "ModeEndingVideo.h"
#include <algorithm>

bool ModeEndingText::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;

	_fadeOutStarted = false;
	_textStarted = false;
	_bgmStarted = false;

	_charIndex = 0;
	_charTimer = 0;
	_textFullyShown = false;

	_text =
		"｢白雪は旧校舎の奥で気を失っていた。\n"
		"意識を取り戻した後に話を聞いたが、どうやら記憶が曖昧みたいだった。\n"
		"そして、この日を境に俺とクロがいろんなトラブルに巻き込まれることになるのは\nまた別のお話ー。";

	// BGM をシナリオ開始時に一度だけ再生する
	//SoundServer::GetInstance()->Play("BGM_Ending", DX_PLAYTYPE_LOOP);

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
	if(_textStarted)
	{
		if(!_bgmStarted)
		{
			SoundServer::GetInstance()->Play("BGM_Ending", DX_PLAYTYPE_LOOP);
			_bgmStarted = true;
		}
		// ...以下既存処理...
	}

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
	

	if(im.IsTrigger(INPUT_ACTION::SKIP))
	{
		if(!_textFullyShown)
		{
			_charIndex = static_cast<int>(_text.size());
			_textFullyShown = true;
		}
		else
		{
		

			// もう一回確定でLOGOへ
			ModeServer::GetInstance()->Clear();
			ModeServer::GetInstance()->Add(new ModeEndingVideo(), 100, "Video");
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

		SetFontSize(40);
		DrawFormatString(120, 780, GetColor(255, 255, 255), "%s", display.c_str());
		SetFontSize(16);
	}

	return true;
}