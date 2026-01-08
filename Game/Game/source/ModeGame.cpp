
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"

#include "PlayerBase.h"
#include "StageBase.h"
#include "CameraManager.h"

// いったんこれ
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"

bool ModeGame::Initialize() 
{
	if (!base::Initialize()) { return false; }

	// いったんこれ
	//_player = std::make_shared<SurfacePlayer>();
	_player = std::make_shared<InteriorPlayer>();
	_player->Initialize();
	_stage = std::make_shared<StageBase>();
	_cameraManager = std::make_shared<CameraManager>();
	_cameraManager->SetTarget(_player);

	return true;
}

bool ModeGame::Terminate() 
{
	base::Terminate();


	// いったんこれ
	_player.reset();

	return true;
}

bool ModeGame::Process()
{
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
		float analogMin = ApplicationMain::GetInstance()->GetAnalogMin();

		// プレイヤーに入力状態を渡す
		if (_player) 
		{
			_player->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if (_cameraManager)
		{
			_cameraManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
	}

	_player->Process();
	_stage->Process();
	_cameraManager->Process();

	return true;
}

bool ModeGame::Render() 
{
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	{
		SetUseLighting(TRUE);

		#if 0	// 平行ライト
			SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
			ChangeLightTypeDir(VGet(-1, -1, 0));
		#endif
		#if 1	// ポイントライト
			SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
			ChangeLightTypePoint(VAdd(_player->GetPos(), VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
		#endif
	}

	// いったんこれ
	_cameraManager->SetUp();
	
	_stage->Render();
	_player->Render();

	return true;
}

