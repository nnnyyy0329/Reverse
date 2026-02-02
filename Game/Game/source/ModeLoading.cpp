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
		{
			rs->Register("Laser", "res/effect/Laser01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

			// 表プレイヤーの攻撃エフェクト
			rs->Register("SurfacePlayerAttack1", "res/effect/Absorb_Attack_00.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("SurfacePlayerAttack2", "res/effect/Absorb_Attack_01.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("SurfacePlayerAttack3", "res/effect/Absorb_Attack_02.efkefc", RESOURCE_TYPE::Effect, 20.0f);

			// 表プレイヤーの攻撃ヒットエフェクト
			rs->Register("SurfacePlayerAttackHit1", "res/effect/Absorb_Hit_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("SurfacePlayerAttackHit2", "res/effect/Absorb_Hit_01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

		}

		// 敵関連
		{
			// モデル
			rs->Register("Melee", "res/SK_enemy_00_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("Ranged", "res/toki_teki_ver006.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ライフバー
			rs->Register("LifeBar", "res/EnemyLifeBar.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("LifeBarFrame", "res/EnemyLifeBarFrame.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// プレイヤーモデル
		rs->Register("SurfacePlayer", "res/Player/Player_00_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
		rs->Register("InteriorPlayer", "res/Player/player_01_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
		//rs->Register("SurfacePlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);
		//rs->Register("InteriorPlayer", "res/SDChar/SDChar.mv1", RESOURCE_TYPE::Model, 1.0f);

		// 能力選択画面用
		rs->Register("select1", "res/AbilitySelect/select1.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select2", "res/AbilitySelect/select2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("select3", "res/AbilitySelect/select4.png", RESOURCE_TYPE::Graph, 1.0f);

		// エネルギーUI用
		rs->Register("energyFrame", "res/Energy/energy_frame_2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy1", "res/Energy/energy_1_2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy2", "res/Energy/energy_2_2.png", RESOURCE_TYPE::Graph, 1.0f);
		rs->Register("energy3", "res/Energy/energy_3_2.png", RESOURCE_TYPE::Graph, 1.0f);

		//rs->Register("energy1", "res/try3/energy_1.png", RESOURCE_TYPE::Graph, 1.0f);
		//rs->Register("energy2", "res/try3/energy_2.png", RESOURCE_TYPE::Graph, 1.0f);
		//rs->Register("energy3", "res/try3/energy_3.png", RESOURCE_TYPE::Graph, 1.0f);
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