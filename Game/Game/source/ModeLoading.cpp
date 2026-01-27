#include "ModeLoading.h"
#include "ModeGame.h"

bool ModeLoading::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddGame = false;

	// リソースの登録
	{
		auto rs = ResourceServer::GetInstance();
		// エフェクト
		rs->Register("Laser", "res/Laser01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

		// 敵関連
		{
			// モデル
			rs->Register("Enemy", "res/toki_teki_ver006.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ライフバー
			rs->Register("LifeBar", "res/EnemyLifeBar.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("LifeBarFrame", "res/EnemyLifeBarFrame.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// プレイヤー関連
		rs->Register("SurfacePlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);
		rs->Register("InteriorPlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);

		// 能力選択画面用
		rs->Register("select1", "res/try3/select1.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select2", "res/try3/select2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select3", "res/try3/select3.png", RESOURCE_TYPE::Graph, 1.0f);

		// エネルギーUI用
		rs->Register("energyFrame", "res/try3/energy_frame.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy1", "res/try3/energy_1try.png", RESOURCE_TYPE::Graph, 1.0f);
		//rs->Register("energy1", "res/try3/energy_1.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy2", "res/try3/energy_2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy3", "res/try3/energy_3.png", RESOURCE_TYPE::Graph, 1.0f);
	}

	// リソースのロード開始
	ResourceServer::GetInstance()->StartLoadAsync();
	return true;
}

bool ModeLoading::Terminate() 
{
	base::Terminate();
	return true;
}

bool ModeLoading::Process()
{
	// ロード進捗の確認
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();

	// ロードが完了した後の処理をここに追加
	if (!_bIsAddGame && ResourceServer::GetInstance()->IsLoadComplete()) {
		_bIsAddGame = true;
		// モードゲームの登録
		ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
	}

	return true;
}

bool ModeLoading::Render()
{
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();

	// 進捗率を描画

	return true;
}