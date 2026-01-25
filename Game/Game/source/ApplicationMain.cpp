#include "appframe.h"
#include "ApplicationMain.h"
#include "ModeLoading.h"


// 消す
#include "ApplicationGlobal.h"


// 実体
ApplicationMain				g_oApplicationMain;

bool ApplicationMain::Initialize(HINSTANCE hInstance) {
	if (!base::Initialize(hInstance)) { return false; }

	// モードの登録
	//ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");



	// 消す
	gGlobal.Init();



	// 非同期ロード用(test)
	{
		auto rs = ResourceServer::GetInstance();

		// リストに登録
		rs->Register("Laser", "res/Laser01.efkefc", RESOURCE_TYPE::Effect, 10.0f);
		rs->Register("LifeBar", "res/EnemyLifeBar.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("LifeBarFrame", "res/EnemyLifeBarFrame.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("SurfacePlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);
		rs->Register("InteriorPlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);

		rs->Register("select1", "res/try3/select1.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select2", "res/try3/select2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select3", "res/try3/select3.png", RESOURCE_TYPE::Graph, 1.0f);

		rs->Register("energyFrame", "res/try3/energy_frame.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy1", "res/try3/energy_1.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy2", "res/try3/energy_2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy3", "res/try3/energy_3.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("tryEnergy1", "res/try3/energy_1try.png", RESOURCE_TYPE::Graph, 1.0f);

		// モードローディングの登録
		ModeServer::GetInstance()->Add(new ModeLoading(), 100, "loading");

	}

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

