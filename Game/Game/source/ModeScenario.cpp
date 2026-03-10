#include "ModeScenario.h"
#include "ModeGame.h"
#include <string>
#include <algorithm>

bool ModeScenario::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;
	_pageIndex = 0;
	_bNext = false;

	_charIndex = 0;
	_charTimer = 0;
	_textFullyShown = false;
	_loadedCount = 0;

	// セリフを設定（ユーザー提供文）
	_texts[0] = "｢なぁ、エイトもう帰ろうぜ〜、見たいテレビがあるんだよな〜。」";
	_texts[1] = "｢ちょっ、クロ！あんま外で喋んなって言ってるだろ！\nそれに白雪に借りたノートを返さないきゃだし。」";
	_texts[2] = "信じて貰えないかもしれないが俺の左腕には自分とは別の意思がある。\n正確には幼い頃の事故でできた黒い\"痣\"なんだが、\n今までは何ともなかったのに突然喋りだしたときは正直かなり驚いた。\n今はもう慣れてとりあえずクロと呼んでいる。";
	_texts[3] = "｢エイトとやたら仲良いあの嬢ちゃんか。」";
	_texts[4] = "｢幼なじみだからな、\nテスト前に借りたノートを返さないと白雪が困るだろ。」";
	_texts[5] = "｢でもよぉ、下の階にもいなかったしこんなとこにホントにいるのか？\nこっちの方に行ってたって言ったヤツの見間違」";
	_texts[6] = "｢？、どうしたんだ？｣";
	_texts[7] = "｢いや、エイトやっぱり進もうぜ。\nこの奥からうまそ、、、同族の匂いがした。」";
	_texts[8] = "｢なんだって、クロに仲間がいるのか？｣";
	_texts[9] = "｢そんなんじゃねぇよ。とにかくホントに嬢ちゃんがいるなら急いだがいいぜ、\nエイトお前も用心しろよ、とっておきの力を貸してやるから」";

	// 画像リソース取得（命名規則: "GameStartText", "GameStartText1", ...）
	for(int i = 0; i < TEXT_COUNT; ++i)
	{
		_textHandles[i] = -1;
		std::string name = (i == 0) ? "GameStartText" : ("GameStartText" + std::to_string(i));
		int h = ResourceServer::GetInstance()->GetHandle(name);
		_textHandles[i] = h;
		if(h >= 0) { ++_loadedCount; }
	}

	// 読み込めた画像がない場合は安全に全件扱い（状況に合わせて変更可）
	if(_loadedCount == 0) { _loadedCount = TEXT_COUNT; }

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

	auto& im = InputManager::GetInstance();

	const bool canInput = (_frameCount >= kInputWaitFrames);

	// 現在ページのテキスト参照
	const std::string& curText = (_pageIndex >= 0 && _pageIndex < TEXT_COUNT) ? _texts[_pageIndex] : std::string();

	// 文字自動展開
	if(!_textFullyShown)
	{
		_charTimer++;
		if(_charTimer >= kCharInterval)
		{
			_charTimer = 0;
			if(_charIndex < static_cast<int>(curText.size()))
			{
				++_charIndex;
			}
			if(_charIndex >= static_cast<int>(curText.size()))
			{
				_textFullyShown = true;
			}
		}
	}

	// 入力: 全文表示済みでなければ全文を出す、済みなら次のページへ
	if(canInput && (input->IsTrigger(INPUT_ACTION::SELECT_POWER) || input->IsTrigger(INPUT_ACTION::ATTACK)))
	{
		if(!_textFullyShown)
		{
			_charIndex = static_cast<int>(curText.size());
			_textFullyShown = true;
		}
		else
		{
			// 次ページへ
			_pageIndex++;
			_frameCount = 0;

			_charIndex = 0;
			_charTimer = 0;
			_textFullyShown = false;

			// 最後のページを超えたらゲームへ
			if(_pageIndex >= TEXT_COUNT)
			{
				ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
				ModeServer::GetInstance()->Del(this);
			}
		}
	}

	return true;
}

bool ModeScenario::Render()
{
	ClearDrawScreen();

	// 画像描画（ページ範囲内ならそのハンドルを描画）
	const int handle = (_pageIndex >= 0 && _pageIndex < TEXT_COUNT) ? _textHandles[_pageIndex] : -1;
	if(handle >= 0)
	{
		int w = 0;
		int h = 0;
		GetGraphSize(handle, &w, &h);

		const int x = (1920 - w) / 2;
		const int y = (1080 - h) / 2;

		DrawGraph(x, y, handle, TRUE);
	}

	// セリフ表示（1文字ずつ）
	const std::string& curText = (_pageIndex >= 0 && _pageIndex < TEXT_COUNT) ? _texts[_pageIndex] : std::string();
	int showCount = std::max(0, std::min(_charIndex, static_cast<int>(curText.size())));
	std::string display = curText.substr(0, showCount);

	// 表示位置は必要に応じて調整してください
	SetFontSize(28);
	DrawFormatString(320, 820, GetColor(255, 255, 255), "%s", display.c_str());
	SetFontSize(16);



	return true;
}