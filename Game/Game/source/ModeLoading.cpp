#include "ModeLoading.h"
#include "ModeGame.h"

bool ModeLoading::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddGame = false;
	_frameCount = 0;

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

			// 敵
			// 共通
			rs->Register("En_Damage", "res/effect/Enemy_Damage_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			// 近距離
			rs->Register("Melee_Attack", "res/effect/NEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			// 遠距離
			rs->Register("Ranged_Attack", "res/effect/SEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
		}

		// 敵関連
		{
			// モデル
			rs->Register("Melee", "res/Enemy/SK_enemy_00_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("Ranged", "res/Enemy/toki_teki_ver006.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ライフバー
			rs->Register("LifeBar", "res/GameUI/EnemyLifeBar.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("LifeBarFrame", "res/GameUI/EnemyLifeBarFrame.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// プレイヤー関連
		{
			// プレイヤーモデル
			rs->Register("SurfacePlayer", "res/Player/SK_player_00_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("InteriorPlayer", "res/Player/SK_player_01_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("BulletPlayer", "res/Player/SK_player_02_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ライフバー
			rs->Register("PlayerLifeBar", "res/Player/UI/Player_Life_Bar.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("PlayerLifeBarFrame", "res/Player/UI/Player_Life_Bar_Frame.png", RESOURCE_TYPE::Graph, 1.0f);

		}

		// 能力選択画面用
		{
			rs->Register("select1", "res/SelectItem/normal.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("select2", "res/SelectItem/melee.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("select3", "res/SelectItem/bullet.png", RESOURCE_TYPE::Graph, 1.0f);
			//rs->Register("select1", "res/AbilitySelect/select1.png", RESOURCE_TYPE::Graph, 1.0f);
			//rs->Register("select2", "res/AbilitySelect/select2.png", RESOURCE_TYPE::Graph, 1.0f);
			//rs->Register("select3", "res/AbilitySelect/select4.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// エネルギーUI用
		{
			rs->Register("EnergyFrame", "res/Energy/energy_frame_2.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy1", "res/Energy/energy_1_2.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy2", "res/Energy/energy_2_2.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy3", "res/Energy/energy_3_2.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// スタミナUI用
		{
			rs->Register("StaminaFrame", "res/StaminaUI/staminaFrame.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Stamina", "res/StaminaUI/Stamina.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// ステージモデル
		{
			// ステージ1のモデル
			rs->Register("stage_fioor_1", "res/stage/json/stage_fioor_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_wall_type_1", "res/stage/json/stage_wall_type_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_wall_typeDoor_1", "res/stage/json/stage_wall_typeDoor_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_wall_typeOutside_1", "res/stage/json/stage_wall_typeOutside_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_wall_typeOutside_2", "res/stage/json/stage_wall_typeOutside_2.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_pillar_1", "res/stage/json/stage_pillar_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage_door_close_1", "res/stage/json/stage_door_close_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("object_tana", "res/stage/json/object_tana.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("object_locker", "res/stage/json/object_locker.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("object_kyodan", "res/stage/json/object_kyodan.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("object_blackboard", "res/stage/json/object_blackboard.mv1", RESOURCE_TYPE::Model, 1.0f);
			// ポータル
			rs->Register("S_Portal_0to1", "res/stage/json/S_Portal_0to1.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ステージ2のモデル
			rs->Register("stage2_floor_1", "res/stage/stage2_floor_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("stage2_wall_1", "res/stage/stage2_wall_1.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("S_portal_0", "res/stage/S_portal_0.mv1", RESOURCE_TYPE::Model, 1.0f);
		}

		// 音関連
		{
			// BGM
			rs->Register("BGM_Stage01", "sound/BGM/stage_01.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("BGM_Stage02", "sound/BGM/stage_02.mp3", RESOURCE_TYPE::Sound, 1.0f);

			// SE
			rs->Register("SE_En_Damage", "sound/SE/en_damage.mp3", RESOURCE_TYPE::Sound, 1.0f);
		}
	}

	// リソースのロード開始
	ResourceServer::GetInstance()->StartLoadAsync();
	return true;
}

bool ModeLoading::Terminate() 
{
	base::Terminate();

	ResourceServer::GetInstance()->Terminate();// リソースサーバーの終了

	return true;
}

bool ModeLoading::Process()
{
	_frameCount++;

	// ロードが完了かつ10フレーム経過後にゲームモードを追加
	if (!_bIsAddGame && ResourceServer::GetInstance()->IsLoadComplete() && _frameCount >= 10) {
		_bIsAddGame = true;
		ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
	}
	return true;
}

bool ModeLoading::Render()
{
	if (_bIsAddGame) { return true; }

	auto progress = ResourceServer::GetInstance()->GetLoadProgress();
	DrawFormatString(640, 340, GetColor(255, 255, 255), "Now Loading...");
	DrawFormatString(640, 380, GetColor(255, 255, 255), "Progress: %.2f%%", progress * 100.0f);

	return true;
}