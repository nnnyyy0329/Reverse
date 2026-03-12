#include "ModeTextBox.h"

ModeTextBox::ModeTextBox(const std::string& graphKey, std::function<void()> onClosed)
	: _graphKey(graphKey)
	, _text()
	, _onClosed(std::move(onClosed))
{
}

ModeTextBox::ModeTextBox(const std::string& graphKey, const std::string& text, std::function<void()> onClosed)
	: _graphKey(graphKey)
	, _text(text)
	, _onClosed(std::move(onClosed))
{
}
bool ModeTextBox::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;
	_bClose = false;

	_graphHandle = ResourceServer::GetInstance()->GetHandle(_graphKey);

	
	return true;
}

bool ModeTextBox::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeTextBox::Process()
{
	_frameCount++;

	// 下のゲームを止める
	ModeServer::GetInstance()->PauseProcessUnderLayer();

	auto& im = InputManager::GetInstance();

	const int kInputWaitFrames = 10;
	const bool canInput = (_frameCount >= kInputWaitFrames);

	if(canInput && (im.IsTrigger(INPUT_ACTION::SELECT_POWER) || im.IsTrigger(INPUT_ACTION::ATTACK)))
	{
		_bClose = true;
	}

	if(_bClose)
	{
		if(_onClosed) { _onClosed(); }
		ModeServer::GetInstance()->Del(this);
	}

	return true;
}

bool ModeTextBox::Render()
{
	if(_graphHandle < 0)
	{
		DrawFormatString(20, 20, GetColor(255, 0, 0), "TextBox Graph Not Found: %s", _graphKey.c_str());
		return true;
	}



	int w = 0;
	int h = 0;
	GetGraphSize(_graphHandle, &w, &h);

	const int screenW = 1920;
	const int screenH = 1080;

	const int marginSide = 80;
	const int marginBottom = 40;

	const int maxW = screenW - marginSide * 2;
	const int maxH = 220; // ← ここを小さくするほど表示も小さくなる

	float scaleW = static_cast<float>(maxW) / static_cast<float>(w);
	float scaleH = static_cast<float>(maxH) / static_cast<float>(h);
	float scale = (scaleW < scaleH) ? scaleW : scaleH;

	if(scale > 1.0f) { scale = 1.0f; }

	const int drawW = static_cast<int>(w * scale);
	const int drawH = static_cast<int>(h * scale);

	const int x = (screenW - drawW) / 2;
	const int y = screenH - drawH - marginBottom;

	DrawExtendGraph(x, y, x + drawW, y + drawH, _graphHandle, TRUE);

	// テキスト描画（追加）
	if(!_text.empty())
	{
		SetFontSize(32);

		const int textX = x + 120;
		const int textY = y + 70;

		// アウトライン（黒）
		const int colOutline = GetColor(0, 0, 0);
		for(int oy = -2; oy <= 2; ++oy)
		{
			for(int ox = -2; ox <= 2; ++ox)
			{
				if(ox == 0 && oy == 0) { continue; }
				DrawFormatString(textX + ox, textY + oy, colOutline, "%s", _text.c_str());
			}
		}

		// 本体（白）
		DrawFormatString(textX, textY, GetColor(255, 255, 255), "%s", _text.c_str());

		SetFontSize(16);
	}

	return true;
}