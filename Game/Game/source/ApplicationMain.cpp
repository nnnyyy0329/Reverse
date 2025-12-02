
#include "ApplicationMain.h"

// 実体
ApplicationMain				g_oApplicationMain;

// 初期化
bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	_cg = LoadGraph("res/player00.png");
	_x = 0;
	_y = 0;
	_w = 48;
	_h = 48;

	return true;
}

// 終了
bool ApplicationMain::Terminate() {
	DeleteGraph(_cg);
	base::Terminate();
	return true;
}

// 入力
bool ApplicationMain::Input() {
	base::Input();
	return true;
}

// 更新
bool ApplicationMain::Process() {
	base::Process();

	if (_gKey & PAD_INPUT_LEFT)	{ _x -= 8; }
	if (_gKey & PAD_INPUT_RIGHT){ _x += 8; }
	if (_gKey & PAD_INPUT_UP)	{ _y -= 8; }
	if (_gKey & PAD_INPUT_DOWN)	{ _y += 8; }

	// 画面外に出ないようにする
	if (_x < 0) { _x = 0; }
	if (_y < 0) { _y = 0; }
	if (_x > DispSizeW() - _w) { _x = DispSizeW() - 64; }
	if (_y > DispSizeH() - _h) { _y = DispSizeH() - 64; }

	return true;
}

// 描画
bool ApplicationMain::Render() {
	base::Render(); // 呼び出し

	DrawGraph(_x, _y, _cg, TRUE);

	return true;
}

