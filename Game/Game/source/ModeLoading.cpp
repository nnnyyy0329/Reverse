#include "ModeLoading.h"
#include "ModeScenario.h"
#include "ModeGame.h"

bool ModeLoading::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddGame = false;
	_frameCount = 0;

	_seHandle = LoadSoundMem("sound/SE/loading.mp3");
	//PlaySoundMem(_seHandle, DX_PLAYTYPE_BACK);

	// リソースの登録
	{
		auto rs = ResourceServer::GetInstance();

		// エフェクト
		{
			rs->Register("Laser", "effect/Laser01.efkefc", RESOURCE_TYPE::Effect, 10.0f);


			/* プレイヤー */

			// 表プレイヤーの吸収攻撃エフェクト
			rs->Register("SurfacePlayerAbsorb", "effect/SurfacePlayer/Absorb_Hit_01.efkefc", RESOURCE_TYPE::Effect, 10.0f);

			// 裏プレイヤーの攻撃エフェクト
			rs->Register("InteriorPlayerAttack_123", "effect/InteriorPlayer/NChange_Attack.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("InteriorPlayerFourthAttack", "effect/InteriorPlayer/NChange_Fourth_Attack.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("InteriorPlayerFifthAttack", "effect/InteriorPlayer/NChange_Attack_Scallup.efkefc", RESOURCE_TYPE::Effect, 20.0f);

			// 表プレイヤーの攻撃ヒットエフェクト
			rs->Register("SurfacePlayerAttackHit1", "effect/Absorb_Hit_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("SurfacePlayerAttackHit2", "effect/Absorb_Hit_01.efkefc", RESOURCE_TYPE::Effect, 10.0f);


			// 敵
			// 共通
			rs->Register("En_Damage", "effect/Enemy_Damage_00.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("En_Damage02", "effect/en_damage_test.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			rs->Register("En_Dead02", "effect/en_dead_test.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			// 一般
			rs->Register("Normal_Attack", "effect/NEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 10.0f);
			rs->Register("Normal_Attack02", "effect/en_attack_test.efkefc", RESOURCE_TYPE::Effect, 20.0f);
			// 遠距離
			rs->Register("Ranged_Attack", "effect/SEnemy_Attack_00.efkefc", RESOURCE_TYPE::Effect, 20.0f);
		}

		// 敵関連
		{
			// モデル
			rs->Register("Melee", "res/Enemy/SK_enemy_00_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("Ranged", "res/Enemy/SK_enemy_01_multimotion.mv1", RESOURCE_TYPE::Model, 1.0f);

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
			rs->Register("PlayerLifeBar", "res/Player/UI/player_life_bar.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("PlayerLifeBarFrame", "res/Player/UI/player_life_bar_frame.png", RESOURCE_TYPE::Graph, 1.0f);

		}

		// 能力選択画面用
		{
			rs->Register("select1", "res/SelectItem/normal.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("select2", "res/SelectItem/melee.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("select3", "res/SelectItem/bullet.png", RESOURCE_TYPE::Graph, 1.0f);

			rs->Register("SelectChain", "res/UI_Select/ability_lock_chain.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SelectPower", "res/UI_Select/select_power.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SelectBlaster", "res/UI_Select/select_blaster.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SkillAbsorb", "res/UI_Select/skill_absorb.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SkillAoe", "res/UI_Select/skill_aoe.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SkillPunch", "res/UI_Select/skill_punch.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SkillPiercing", "res/UI_Select/skill_piercing.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("SkillNormal", "res/UI_Select/skill_normal.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// 入力UI 
		{
			rs->Register("InputA", "res/UI_Input/input_A.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("InputB", "res/UI_Input/input_B.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("InputX", "res/UI_Input/input_X.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("InputY", "res/UI_Input/input_Y.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("InputLT", "res/UI_Input/input_LT.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("InputRT", "res/UI_Input/input_RT.png", RESOURCE_TYPE::Graph, 1.0f);
		}

		// エネルギーUI用
		{
			rs->Register("EnergyFrame", "res/Energy/energy_frame.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy1", "res/Energy/energy_1r.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy2", "res/Energy/energy_2r.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("Energy3", "res/Energy/energy_3r.png", RESOURCE_TYPE::Graph, 1.0f);
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
			// 敵の移動可能範囲
			rs->Register("Enemy_Area", "res/stage/json/Enemy_Area.mv1", RESOURCE_TYPE::Model, 1.0f);

			// ステージ2のモデル
			rs->Register("SM_MERGED_BigB", "res/stage/json/SM_MERGED_BigB.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("SM_walltypeZ", "res/stage/json/SM_walltypeZ.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("SM_MERGED_BigD", "res/stage/json/SM_MERGED_BigD.mv1", RESOURCE_TYPE::Model, 1.0f);
			rs->Register("SM_MERGED_locker", "res/stage/json/SM_MERGED_locker.mv1", RESOURCE_TYPE::Model, 1.0f);
			// ポータル
			rs->Register("S_Portal_1to2", "res/stage/json/S_Portal_1to2.mv1", RESOURCE_TYPE::Model, 1.0f);
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

			// プレイヤー変身
			rs->Register("ChangePower", "sound/SE/changePlayer/knuckleSE.mp3", RESOURCE_TYPE::Sound, 1.0f);
			rs->Register("ChangeBlaster", "sound/SE/changePlayer/blasterSE.mp3", RESOURCE_TYPE::Sound, 1.0f);

		}

		// シナリオ　
		{
			rs->Register("GameStartText ",  "res/Senario/story1.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText1",  "res/Senario/story2.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText2",  "res/Senario/story3.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText3",  "res/Senario/story4.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText4",  "res/Senario/story5.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText5",  "res/Senario/story6.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText6",  "res/Senario/story7.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText7",  "res/Senario/story8.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText8",  "res/Senario/story9.png",  RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText9",  "res/Senario/story10.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText10", "res/Senario/story11.png", RESOURCE_TYPE::Graph, 1.0f);
			rs->Register("GameStartText11", "res/Senario/story12.png", RESOURCE_TYPE::Graph, 1.0f);
		}
		
	    //　テキストボックス　
		{

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

		// 進行度
		const int filled = static_cast<int>(progress * static_cast<float>(blocks) );    
		// ブロックの色
		const int colFilled = GetColor(0, 120, 255);                                    
		const int colEmpty = GetColor(170, 170, 170);
		// ブロックの間隔
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

	//ResourceServer::GetInstance()->Terminate();  // リソースサーバーの終了

	return true;
}

bool ModeLoading::Process()
{

	_frameCount++;
	

	//デバッグ用ローディング終わってもゲームに行かないコード
	/*if(ResourceServer::GetInstance()->IsLoadComplete())
	{
		return true;
	}*/

	// ロードが完了かつ10フレーム経過後にゲームモードを追加
	if(!_bIsAddGame && ResourceServer::GetInstance()->IsLoadComplete() && _frameCount >= 10)
	{
		StopSoundMem(_seHandle);
		_bIsAddGame = true;

		ModeServer::GetInstance()->Add(new ModeScenario(), 1, "scenario");
		ModeServer::GetInstance()->Del(this);
	}
	return true;

	
}







bool ModeLoading::Render()
{
	if (_bIsAddGame) { return true; }
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();


	//ブロック
	{
		const int blocks = 26;
		const int blockW = 12;
		const int blockH = 36;
		const int gap = 6;

		const int barX = 420;
		const int barY = 520;

		DrawBlockLoadingBar(barX, barY, blocks, blockW, blockH, gap, progress);
	}
	


	//nowloading    XXX %
	{
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
				DrawFormatString(textX + dx, textY + dy, glowColor, "now loading");
				DrawFormatString(textX + dx, textY + 50 + dy, glowColor, "Progress: %.2f%%", progress * 100.0f);
			}
		}

		// 本体（中心）
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);


		SetFontSize(16);
	}

	return true;
}