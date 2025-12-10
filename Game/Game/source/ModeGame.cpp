#include "ApplicationMain.h"
#include "ModeGame.h"
#include "PlayerBase.h"
#include "StageBase.h"

bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	_player = std::make_shared<PlayerBase>();
	_stage = std::make_shared<StageBase>();

	return true;
}

bool ModeGame::Terminate() {
	base::Terminate();
	return true;
}

bool ModeGame::Process() {
	base::Process();
	/// 入力取得
	{
		int key = ApplicationMain::GetInstance()->GetKey();
		int trg = ApplicationMain::GetInstance()->GetTrg();
		auto analog = ApplicationMain::GetInstance()->GetAnalog();
		float lx = analog.lx;
		float ly = analog.ly;
		float rx = analog.rx;
		float ry = analog.ry;

		// プレイヤーに入力状態を渡す
		if (_player) {
			_player->SetInput(key, trg, lx, ly, rx, ry);
		}
	}

	_stage->Process();

	return true;
}

bool ModeGame::Render() {
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	SetUseLighting(TRUE);
	VECTOR vLightDir = VGet(0.f, -1, 0.2f);
	// 平行ライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypeDir(vLightDir);

	_stage->Render();

	return true;
}
