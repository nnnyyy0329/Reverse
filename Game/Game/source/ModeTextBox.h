#pragma once
#include "appframe.h"

#include <functional>
#include <string>
#include <vector>

class ModeTextBox : public ModeBase
{
public:
	typedef ModeBase base;

	// 既存互換コンストラクタ（グラフのみ）
	explicit ModeTextBox(const std::string& graphKey, std::function<void()> onClosed = nullptr, bool pauseUnderLayer = true);

	// グラフ + 表示テキスト（改行対応）。pauseUnderLayer=false にするとゲームは継続して進行します
	explicit ModeTextBox(const std::string& graphKey, const std::string& text, std::function<void()> onClosed = nullptr, bool pauseUnderLayer = true);

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

	// 静的ヘルパー: 単体表示（pauseUnderLayer = false にしてゲームを止めない）
	static void Show(const std::string& graphKey, const std::string& text, bool pauseUnderLayer = true, int z = 100, const std::string& instanceName = "textbox");

	// 静的ヘルパー: 連続表示（items: pair<graphKey, text>）
	static void ShowChain(const std::vector<std::pair<std::string, std::string>>& items, bool pauseUnderLayer = true, int z = 100, const std::string& baseName = "textbox_chain");

private:
	std::string _graphKey;
	std::function<void()> _onClosed;

	int _graphHandle = -1;

	int _frameCount = 0;
	bool _bClose = false;

	// 表示するテキスト（任意）
	std::string _text;

	// true のとき下層を停止（既存挙動）。false ならゲームは継続。
	bool _pauseUnderLayer = true;
};