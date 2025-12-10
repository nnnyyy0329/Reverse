#include "appframe.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

// À‘Ì
ApplicationMain				g_oApplicationMain;

bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	// ƒ‚[ƒh‚Ì“o˜^
	ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");

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
	base::Render(); // ŒÄ‚Ño‚µ

	return true;
}

