#pragma once
#include "appframe.h"
#include "ScenarioBase.h"

//前方宣言
class ScenarioBase;

class ModeTitle : public ModeBase 
{
	typedef ModeBase base;
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

protected:

	int _titleHandle;        // タイトル画像ハンドル
	int _alpha;             // フェード用アルファ値
	int _fadeState;         // フェード状態
	int _frameCount;        // フレームカウント

private:

	void Drawgraph(VECTOR pos, std::string name);
	bool _bIsAddLoading = false;
};
