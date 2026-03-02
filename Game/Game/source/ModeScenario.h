#pragma once
#include "appframe.h"


class ModeScenario :public ModeBase
{
public:
	ModeScenario(const std::string& resourceName, int durationFrames = 180, int fadeFrames = 30);
	~ModeScenario();

	void Show();					// 表示開始（フェードイン）
	void Hide();					// 即時非表示（フェードアウトは Skip() を使う）
	void Skip();					// 即時スキップ（フェードアウトへ移行）
	void Update();					// 毎フレーム更新（Process 内から呼ぶ）
	virtual bool Render()override;  // 描画（Render 内から呼ぶ）

	bool IsFinished() const;        // 完全に終了（表示完了して非表示になった）
	int  GetHandle() const { return _handle; }

	// 表示配置など
	void SetPosition(int x, int y) { _x = x; _y = y; }
	void SetScale(float sx, float sy) { _scaleX = sx; _scaleY = sy; }

	void SetAutoClose(bool enable) { _autoClose = enable; }
private:
	enum class State { Idle, FadingIn, Showing, FadingOut, Done };

	std::string _resourceName;
	int _handle;

	State _state;
	int _timer;         // 現在のフェーズ内フレームカウンタ
	int _duration;      // 表示維持フレーム数
	int _fadeFrames;    // フェードイン・アウトに使うフレーム数
	int _alpha;         // 0..255

	int _x, _y;
	float _scaleX, _scaleY;
	bool _visible;
	bool _autoClose;
	// ResourceServer からハンドルを取得（ロードが完了していれば取得される）
	void EnsureHandle();
};



