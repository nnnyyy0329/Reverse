#include "ModeLoading.h"
#include "ModeGame.h"

bool ModeLoading::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddGame = false;
	_frameCount = 0;

	_seHandle = LoadSoundMem("sound/SE/loading.mp3");
	PlaySoundMem(_seHandle, DX_PLAYTYPE_BACK);

	// リソースの登録
	{
		auto rs = ResourceServer::GetInstance();

		

		// エフェクト
		{
			rs->Register("Laser", "effect/Laser01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

			// 表プレイヤーの攻撃エフェクト
			rs->Register("SurfacePlayerAttack1", "effect/sPlayer/Absorb_Attack_00.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("SurfacePlayerAttack2", "effect/sPlayer/Absorb_Attack_01.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("SurfacePlayerAttack3", "effect/sPlayer/Absorb_Attack_02.efkefc", RESOURCE_TYPE::Effect, 20.0f);

			// 表プレイヤーのダメージエフェクト
			rs->Register("InteriorPlayerAttack1", "effect/iPlayer/NChange_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("InteriorPlayerAttack2", "effect/iPlayer/NChange_Attack_01.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("InteriorPlayerAttack3", "effect/iPlayer/NChange_Attack_02.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("InteriorPlayerAttack4", "effect/iPlayer/NChange_Attack_03.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("InteriorPlayerAttack5", "effect/iPlayer/NChange_Attack_04.efkefc", RESOURCE_TYPE::Effect, 10.0f);

			// 表プレイヤーの攻撃ヒットエフェクト
			rs->Register("SurfacePlayerAttackHit1", "effect/Absorb_Hit_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("SurfacePlayerAttackHit2", "effect/Absorb_Hit_01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

			// 敵
			// 共通
			rs->Register("En_Damage", "effect/Enemy_Damage_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			// 近距離
			rs->Register("Melee_Attack", "effect/NEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			// 遠距離
			rs->Register("Ranged_Attack", "effect/SEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
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
			rs->Register("SE_En_Damage", "sound/SE/en_common/en_damage.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("SE_En_Dead", "sound/SE/en_common/en_dead.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("SE_En_Down", "sound/SE/en_common/en_down.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("SE_Melee_Attanck", "sound/SE/en_melee/en_melee_attack.mp3", RESOURCE_TYPE::Sound, 1.0f);

			// プレイヤーの攻撃
			rs->Register("sPlayerFirstAttack", "sound/SE/sPlayerFirstAttack/sPlayerFirstAttack1.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("iPlayerAttack", "sound/SE/iPlayerAttack/iPlayerAttack1.mp3", RESOURCE_TYPE::Sound, 1.0f);
		}

		
	}

	// リソースのロード開始
	ResourceServer::GetInstance()->StartLoadAsync();
	return true;
}

namespace
{
	void DrawBlockLoadingBar(int x, int y, int blocks, int blockW, int blockH, int gap, float progress)
	{
		if(blocks <= 0) { return; }

		if(progress < 0.0f) { progress = 0.0f; }
		if(progress > 1.0f) { progress = 1.0f; }

		const int filled = static_cast<int>(progress * static_cast<float>(blocks) + 0.5f);

		const int colFilled = GetColor(0, 120, 255);
		const int colEmpty = GetColor(170, 170, 170);

		for(int i = 0; i < blocks; ++i)
		{
			const int bx = x + i * (blockW + gap);
			const int by = y;

			const int col = (i < filled) ? colFilled : colEmpty;
			DrawBox(bx, by, bx + blockW, by + blockH, col, TRUE);
		}
	}
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
	// DEBUG: ロード完了してもゲームへ遷移しない（ローディング画面に留まる）
	if(ResourceServer::GetInstance()->IsLoadComplete())
	{
		return true;
	}

	// ロードが完了かつ10フレーム経過後にゲームモードを追加
	if (!_bIsAddGame && ResourceServer::GetInstance()->IsLoadComplete() && _frameCount >= 10) {
		StopSoundMem(_seHandle);
		_bIsAddGame = true;
		ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
	}
	return true;
}

bool ModeLoading::Render()
{
	if (_bIsAddGame) { return true; }
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();


	// -------- ブロック式ローディングバー --------
	const int blocks = 26;
	const int blockW = 12;
	const int blockH = 36;
	const int gap = 6;

	const int barX = 420;
	const int barY = 520;

	DrawBlockLoadingBar(barX, barY, blocks, blockW, blockH, gap, progress);




	SetFontSize(48);
	const int textX = 640;
	const int textY = 640;

	// 光（外側）: 少し透明にして何回か描く
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 80); 
	const int glowColor = GetColor(255, 0, 255);

	for(int dy = -2; dy <= 2; ++dy)
	{
		for(int dx = -2; dx <= 2; ++dx)
		{
			if(dx == 0 && dy == 0) { continue; }
		    DrawFormatString(textX + dx, textY     + dy, glowColor, "now loading");
			DrawFormatString(textX + dx, textY+50 + dy, glowColor, "Progress: %.2f%%", progress * 100.0f);
		}
	}

	// 本体（中心）
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	

	SetFontSize(16);


	return true;
}