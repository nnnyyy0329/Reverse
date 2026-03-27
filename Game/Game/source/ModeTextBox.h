#pragma once
#include "appframe.h"

#include <deque>
#include <functional>
#include <string>
#include <vector>

class ModeTextBox : public ModeBase
{
public:
	typedef ModeBase base;

	// 既存コンストラクタ（互換維持）
	explicit ModeTextBox(const std::string& graphKey, std::function<void()> onClosed = nullptr, bool pauseUnderLayer = true);
	explicit ModeTextBox(const std::string& graphKey, const std::string& text, std::function<void()> onClosed = nullptr, bool pauseUnderLayer = true);

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

	// 静的ヘルパ: 1つ表示（表示中なら順番待ち）
	static void Show(const std::string& graphKey, const std::string& text, bool pauseUnderLayer = true, int z = 100, const std::string& instanceName = "textbox");

	// 静的ヘルパ: 連続表示（表示中ならチェーンごと順番待ち）
	static void ShowChain(const std::vector<std::pair<std::string, std::string>>& items, bool pauseUnderLayer = true, int z = 100, const std::string& baseName = "textbox_chain");

private:
	struct QueueItem
	{
		std::string graphKey;
		std::string text;
		bool pauseUnderLayer = true;
		int z = 100;
		std::string instanceName;
	};

	static void Enqueue(const QueueItem& item);
	static void TryDequeueAndShow();

	static ModeTextBox* _lpActive;
	static std::deque<QueueItem> _queue;

private:
	std::string _graphKey;
	std::string _text;
	std::function<void()> _onClosed;

	int _graphHandle = -1;

	int _frameCount = 0;
	bool _bClose = false;

	// true のとき下層をポーズ（止める）。false ならゲームは継続。
	bool _pauseUnderLayer = true;
};