#pragma once
#include "appframe.h"

class ModeScenario :public ModeBase
{
public:
	typedef ModeBase base;
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	static constexpr int TEXT_COUNT = 10;
	static constexpr int kCharInterval = 2;        // 何フレームごとに次の文字を出すか
	static constexpr int kInputWaitFrames = 10;    // 押下を受け付け始めるまでの待ちフレーム

	int _textHandles[TEXT_COUNT] = { -1 };
	std::string _texts[TEXT_COUNT];
	int _loadedCount = 0; // 実際に読み込めた画像数

	int _pageIndex = 0;
	int _frameCount = 0;

	// テキスト表示制御
	int _charIndex = 0;    // 現在表示している文字数
	int _charTimer = 0;    // フレームカウント（文字切り替え用）
	bool _textFullyShown = false;

	bool _bNext = false;
};


