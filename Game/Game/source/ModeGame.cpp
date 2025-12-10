#include "ApplicationMain.h"
#include "ModeGame.h"
#include "StageBase.h"

bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

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

		// アナログスティック対応
		DINPUT_JOYSTATE di;
		GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
		float lx, ly, rx, ry;// 左右アナログスティックの座標
		float analogMin = 0.3f;// アナログ閾値
		// Logicoolパッドの場合
		lx = (float)di.X / 1000.f; ly = (float)di.Y / 1000.f;// 左スティック
		rx = (float)di.Rx / 1000.f; ry = (float)di.Ry / 1000.f;// 右スティック
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
