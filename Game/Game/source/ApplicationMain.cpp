#include "appframe.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

// 実体
ApplicationMain				g_oApplicationMain;

// 初期化
bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	// モードの登録
	ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");

	return true;
}

// 終了
bool ApplicationMain::Terminate() {
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

	return true;
}

// 描画
bool ApplicationMain::Render() {
	base::Render(); // 呼び出し

	return true;
}

