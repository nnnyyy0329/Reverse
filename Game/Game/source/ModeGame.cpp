
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

// ‚¢‚Á‚½‚ñ‚±‚ê
#include "SurfacePlayer.h"

bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	// ‚¢‚Á‚½‚ñ‚±‚ê
	_surfacePlayer = std::make_shared<SurfacePlayer>();
	_surfacePlayer->Initialize();


	return true;
}

bool ModeGame::Terminate() {
	base::Terminate();


	// ‚¢‚Á‚½‚ñ‚±‚ê
	_surfacePlayer.reset();

	return true;
}

bool ModeGame::Process() {
	base::Process();

	// ‚¢‚Á‚½‚ñ‚±‚ê
	_surfacePlayer->Process();

	return true;
}

bool ModeGame::Render() {
	base::Render();

	// ‚¢‚Á‚½‚ñ‚±‚ê
	_surfacePlayer->Render();

	return true;
}

