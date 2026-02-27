#pragma once
#include "appframe.h"

#include <functional>
#include <string>

class ModeTextBox : public ModeBase
{
public:
	typedef ModeBase base;

	explicit ModeTextBox(const std::string& graphKey, std::function<void()> onClosed = nullptr);

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	std::string _graphKey;
	std::function<void()> _onClosed;

	int _graphHandle = -1;

	int _frameCount = 0;
	bool _bClose = false;
};