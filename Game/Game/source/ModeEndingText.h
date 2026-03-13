#pragma once
#include "appframe.h"

class ModeEndingText : public ModeBase
{
public:
	typedef ModeBase base;

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	static constexpr int kCharInterval = 2;     // 何フレームごとに1文字出す
	static constexpr int kInputWaitFrames = 10; // 連打防止

	std::string _text;
	int _frameCount = 0;

	// フェード制御
	bool _fadeOutStarted = false;
	bool _textStarted = false;

	// テキスト表示制御
	int _charIndex = 0;
	int _charTimer = 0;
	bool _textFullyShown = false;
};