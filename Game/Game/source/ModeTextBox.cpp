#include "ModeTextBox.h"

std::vector<ModeTextBox*> ModeTextBox::_textBox;

ModeTextBox::ModeTextBox(const std::string& graphKey, std::function<void()> onClosed, bool pauseUnderLayer)
	: _graphKey(graphKey)
	, _onClosed(std::move(onClosed))
	, _text()
	, _pauseUnderLayer(pauseUnderLayer)
{
}

ModeTextBox::ModeTextBox(const std::string& graphKey, const std::string& text, std::function<void()> onClosed, bool pauseUnderLayer)
	: _graphKey(graphKey)
	, _onClosed(std::move(onClosed))
	, _text(text)
	, _pauseUnderLayer(pauseUnderLayer)
{
}

ModeTextBox::~ModeTextBox()
{
	// インスタンスが破棄されるとき、静的リストからも削除する
	auto it = std::find(_textBox.begin(), _textBox.end(), this);
	if(it != _textBox.end())
	{
		_textBox.erase(it);
	}
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

	// 下のゲームを止めるかどうかはフラグで制御
	if(_pauseUnderLayer)
	{
		ModeServer::GetInstance()->PauseProcessUnderLayer();
	}

	auto& im = InputManager::GetInstance();

	const int kInputWaitFrames = 10;
	const bool canInput = (_frameCount >= kInputWaitFrames);

	float closeTime = 0.0f;
	// 時間経過で自動的に閉じる場合の処理
	if(_frameCount >= 260)
	{
		closeTime = (_frameCount - 360) / 60.0f;// 6秒経過で完全に閉じる
		if(closeTime >= 1.0f)
		{
			closeTime = 1.0f;
			_bClose = true;
		}
	}

	if(im.IsTrigger(INPUT_ACTION::SKIP))
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

	const int marginSide = 60;
	const int marginBottom = 40;

	const int maxW = screenW - marginSide * 2;
	const int maxH = 800; // ← ここを小さくするほど表示も小さくなる

	float scaleW = static_cast<float>(maxW) / static_cast<float>(w);
	float scaleH = static_cast<float>(maxH) / static_cast<float>(h);
	float scale = (scaleW < scaleH) ? scaleW : scaleH;

	if(scale > 1.0f) { scale = 1.0f; }

	const int drawW = static_cast<int>(w * scale);
	const int drawH = static_cast<int>(h * scale);

	const int x = (screenW - drawW) / 2;
	const int y = screenH - drawH - marginBottom;

	DrawExtendGraph(x, y, x + drawW, y + drawH, _graphHandle, TRUE);

	// テキスト描画（_text が空でなければ描画）
	if(!_text.empty())
	{
		const int textLeft = x + 450;
		const int textTop = y + 650;
		const int lineHeight = 30;

		SetFontSize(22);
		size_t pos = 0;
		size_t lineIndex = 0;
		while(pos <= _text.size())
		{
			size_t next = _text.find('\n', pos);
			std::string line;
			if(next == std::string::npos)
			{
				line = _text.substr(pos);
				pos = _text.size() + 1;
			}
			else
			{
				line = _text.substr(pos, next - pos);
				pos = next + 1;
			}
			DrawFormatString(textLeft, textTop + static_cast<int>(lineIndex * lineHeight), GetColor(255, 255, 255), "%s", line.c_str());
			++lineIndex;
			if(lineIndex > 10) break;
		}
		SetFontSize(16);
	}

	return true;
}

// static helpers

void ModeTextBox::Show(const std::string& graphKey, const std::string& text, bool pauseUnderLayer, int z, const std::string& instanceName)
{
	ModeTextBox* box = new ModeTextBox(graphKey, text, nullptr, pauseUnderLayer);
	ModeServer::GetInstance()->Add(box, z, instanceName.c_str());
	_textBox.push_back(box);
}

void ModeTextBox::ShowChain(const std::vector<std::pair<std::string, std::string>>& items, bool pauseUnderLayer, int z, const std::string& baseName)
{
	if(items.empty()) return;

	// 末尾から先に生成して、前の onClosed で次を追加する方式
	ModeTextBox* nextBox = nullptr;
	for(int i = static_cast<int>(items.size()) - 1; i >= 0; --i)
	{
		const auto& p = items[i];
		std::string name = baseName + "_" + std::to_string(i);

		// 最初は nextBox == nullptr なので、末尾のボックスが作られる。以降は、前のボックスが閉じられたときに nextBox を追加する形になる
		if(nextBox == nullptr)
		{
			// 末尾
			nextBox = new ModeTextBox(p.first, p.second, nullptr, pauseUnderLayer);
			_textBox.push_back(nextBox);
		}
		else
		{
			// 現在のボックスが閉じられたら nextBox を追加する
			ModeTextBox* capturedNext = nextBox;
			auto onClosed = [capturedNext, z, name]() {
				ModeServer::GetInstance()->Add(capturedNext, z, name.c_str());
				};
			// 新しい box を作る（閉じたら capturedNext を追加）
			nextBox = new ModeTextBox(p.first, p.second, onClosed, pauseUnderLayer);
			_textBox.push_back(nextBox);
		}
	}

	// 最初のボックスを追加（名前は baseName_0）
	std::string firstName = baseName + "_0";
	ModeServer::GetInstance()->Add(nextBox, z, firstName.c_str());
}

void ModeTextBox::CloseAll()
{
	// すべてのテキストボックスを走査
	for(ModeTextBox* box : _textBox)
	{
		if(box)
		{
			// 閉じる前のコールバックがあれば呼び出す
			if(box->_onClosed) { box->_onClosed(); }

			// テキストボックスを削除
			ModeServer::GetInstance()->Del(box);
		}
	}

	_textBox.clear();
}