#include "ScenarioBase.h"
#include <DxLib.h>

ScenarioBase::ScenarioBase(const std::string& resourceName, int durationFrames, int fadeFrames)
	: _resourceName(resourceName)
	, _handle(-1)
	, _state(State::Idle)
	, _timer(0)
	, _duration(durationFrames)
	, _fadeFrames(fadeFrames > 0 ? fadeFrames : 1)
	, _alpha(0)
	, _x(0)
	, _y(0)
	, _scaleX(1.0f)
	, _scaleY(1.0f)
	, _visible(false)
	, _autoClose(true)
{
	EnsureHandle();
}

ScenarioBase::~ScenarioBase()
{
	// リソースは ResourceServer 側で管理する想定のためここでは何もしない
}

void ScenarioBase::EnsureHandle()
{
	auto rs = ResourceServer::GetInstance();
	if(rs) 
	{
		_handle = rs->GetHandle(_resourceName);
	}
}

void ScenarioBase::Show()
{
	if(_handle == -1) EnsureHandle();
	_visible = true;
	_state = State::FadingIn;
	_timer = 0;
	_alpha = 0;
}

void ScenarioBase::Hide()
{
	_visible = false;
	_state = State::Done;
	_timer = 0;
	_alpha = 0;
}

void ScenarioBase::Skip()
{
	// 表示中ならフェードアウトへ
	if(_state == State::FadingIn || _state == State::Showing) {
		_state = State::FadingOut;
		_timer = 0;
	}
}

void ScenarioBase::Update()
{
	if(!_visible) return;

	// ハンドルがまだ取れていない場合は再取得を試みる
	if(_handle == -1) EnsureHandle();

	switch(_state)
	{
		case State::FadingIn:
			_timer++;
			_alpha = (_timer * 255) / _fadeFrames;
			if(_alpha > 255) _alpha = 255;
			if(_timer >= _fadeFrames) {
				_state = State::Showing;
				_timer = 0;
			}
			break;

		case State::Showing:
			_timer++;
			// 自動でフェードアウトするのは _autoClose == true の場合のみ
			if(_autoClose && _timer >= _duration) {
				_state = State::FadingOut;
				_timer = 0;
			}
			break;

		case State::FadingOut:
			_timer++;
			_alpha = 255 - (_timer * 255) / _fadeFrames;
			if(_alpha < 0) _alpha = 0;
			if(_timer >= _fadeFrames) {
				_state = State::Done;
				_visible = false;
				_timer = 0;
			}
			break;

		default:
			break;
	}
}

void ScenarioBase::Render()
{
	if(!_visible) return;
	if(_handle == -1) return; // ハンドルがないと描画不可

	// アルファ描画
	if(_alpha != 255) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _alpha);
	}

	// サイズ取得 -> 拡縮して描画（丸め・負スケール反転対応）
	int w = 0, h = 0;
	if(GetGraphSize(_handle, &w, &h) == 0 && w > 0 && h > 0)
	{
		// 四捨五入で整数化
		int dw = static_cast<int>(std::lroundf(w * _scaleX));
		int dh = static_cast<int>(std::lroundf(h * _scaleY));

		int x1 = _x;
		int y1 = _y;
		int x2 = _x + dw;
		int y2 = _y + dh;

		// 負の拡縮（反転）に対応するため座標を入れ替え
		if(x1 > x2) std::swap(x1, x2);
		if(y1 > y2) std::swap(y1, y2);

		// 等倍なら高速パス
		if(dw == w && dh == h) {
			DrawGraph(_x, _y, _handle, TRUE);
		}
		else {
			DrawExtendGraph(x1, y1, x2, y2, _handle, TRUE);
		}
	}
	else {
		DrawGraph(_x, _y, _handle, TRUE);
	}

	if(_alpha != 255) {
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

bool ScenarioBase::IsFinished() const
{
	return _state == State::Done;
}