#include "appframe.h"
#include "ApplicationMain.h"
#include "ModeLoading.h"


// 消す
#include "ApplicationGlobal.h"


// 実体
ApplicationMain				g_oApplicationMain;

bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	// アプリケーショングローバルの初期化
	gGlobal.Init();

	// モードの登録
	//ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
	ModeServer::GetInstance()->Add(new ModeLoading(), 9999, "loading");

	return true;
}

bool ApplicationMain::Terminate() {
	base::Terminate();
	return true;
}


bool ApplicationMain::Input() {
	base::Input();
	return true;
}

bool ApplicationMain::Process() {
	base::Process();

	return true;
}

bool ApplicationMain::Render() {
	base::Render(); // 呼び出し

	return true;
}

