
#include "ApplicationMain.h"
#include "ApplicationGlobal.h"
#include "ModeGame.h"

// ŽÀ‘Ì
ApplicationMain				g_oApplicationMain;

bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	gGlobal.Init();

	// ƒ‚[ƒh‚Ì“o˜^
	ModeServer::GetInstance()->Add(new ModeGame(), 10, "game");

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
	base::Render();
	return true;
}

