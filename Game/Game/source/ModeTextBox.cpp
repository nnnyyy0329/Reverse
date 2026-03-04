#include "ModeTextBox.h"

ModeTextBox::ModeTextBox(const std::string& graphKey, std::function<void()> onClosed)
	: _graphKey(graphKey)
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

	// ‰º‚ÌƒQ[ƒ€‚ðŽ~‚ß‚é
	ModeServer::GetInstance()->PauseProcessUnderLayer();

	auto input = InputManager::GetInstance();

	const int kInputWaitFrames = 10;
	const bool canInput = (_frameCount >= kInputWaitFrames);

	if(canInput && (input->IsTrigger(INPUT_ACTION::SELECT) || input->IsTrigger(INPUT_ACTION::ATTACK)))
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
	const int maxH = 220; // © ‚±‚±‚ð¬‚³‚­‚·‚é‚Ù‚Ç•\Ž¦‚à¬‚³‚­‚È‚é

	float scaleW = static_cast<float>(maxW) / static_cast<float>(w);
	float scaleH = static_cast<float>(maxH) / static_cast<float>(h);
	float scale = (scaleW < scaleH) ? scaleW : scaleH;

	if(scale > 1.0f) { scale = 1.0f; }

	const int drawW = static_cast<int>(w * scale);
	const int drawH = static_cast<int>(h * scale);

	const int x = (screenW - drawW) / 2;
	const int y = screenH - drawH - marginBottom;

	DrawExtendGraph(x, y, x + drawW, y + drawH, _graphHandle, TRUE);

	return true;
}