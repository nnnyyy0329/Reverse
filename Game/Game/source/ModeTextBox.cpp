#include "ModeTextBox.h"

ModeTextBox* ModeTextBox::_lpActive = nullptr;
std::deque<ModeTextBox::QueueItem> ModeTextBox::_queue;

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

bool ModeTextBox::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;
	_bClose = false;

	_graphHandle = ResourceServer::GetInstance()->GetHandle(_graphKey);

	// 念のため（Add経由以外で生成されても破綻しにくくする）
	if(_lpActive == nullptr)
	{
		_lpActive = this;
	}

	return true;
}

bool ModeTextBox::Terminate()
{
	// 外部要因で消された場合でもキューが進むようにする
	if(_lpActive == this)
	{
		_lpActive = nullptr;
		TryDequeueAndShow();
	}

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
		closeTime = (_frameCount - 360) / 60.0f;// 300フレーム(5秒)経過後、1秒かけて閉じる
		if(closeTime >= 1.0f)
		{
			closeTime = 1.0f;
			_bClose = true;
		}
	}

	if(canInput && im.IsTrigger(INPUT_ACTION::SKIP))
	{
		_bClose = true;
	}

	if(_bClose)
	{
		// 「消えてから次を表示」を成立させるため、ここでアクティブ解除 → 次を予約
		if(_lpActive == this)
		{
			_lpActive = nullptr;
		}

		if(_onClosed) { _onClosed(); }

		// 次のテキストボックスを予約（次フレームの ProcessInit で削除→追加の順に処理される）
		TryDequeueAndShow();

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

void ModeTextBox::Enqueue(const QueueItem& item)
{
	_queue.push_back(item);
}

void ModeTextBox::TryDequeueAndShow()
{
	if(_lpActive != nullptr) { return; }
	if(_queue.empty()) { return; }

	const QueueItem item = _queue.front();
	_queue.pop_front();

	ModeTextBox* box = new ModeTextBox(item.graphKey, item.text, nullptr, item.pauseUnderLayer);

	// Addした瞬間に「表示予定」扱いにして、同フレーム中の追加要求をキューに回す
	_lpActive = box;

	ModeServer::GetInstance()->Add(box, item.z, item.instanceName.c_str());
}

void ModeTextBox::Show(const std::string& graphKey, const std::string& text, bool pauseUnderLayer, int z, const std::string& instanceName)
{
	QueueItem item;
	item.graphKey = graphKey;
	item.text = text;
	item.pauseUnderLayer = pauseUnderLayer;
	item.z = z;
	item.instanceName = instanceName;

	Enqueue(item);
	TryDequeueAndShow();
}

void ModeTextBox::ShowChain(const std::vector<std::pair<std::string, std::string>>& items, bool pauseUnderLayer, int z, const std::string& baseName)
{
	if(items.empty()) return;

	for(size_t i = 0; i < items.size(); ++i)
	{
		QueueItem item;
		item.graphKey = items[i].first;
		item.text = items[i].second;
		item.pauseUnderLayer = pauseUnderLayer;
		item.z = z;
		item.instanceName = baseName + "_" + std::to_string(i);

		Enqueue(item);
	}

	TryDequeueAndShow();
}