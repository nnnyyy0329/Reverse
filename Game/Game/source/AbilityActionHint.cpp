#include "AbilityActionHint.h"

// 描画位置設定用の名前空間
namespace RenderConfig
{
	constexpr int DRAW_Y = 750;		// 描画位置Y
}

// アクションヒントの選択インデックス設定用の名前空間
namespace SelectedIndexConfig
{
	constexpr int SURFACE_PLAYER_INDEX = 0;		// 表プレイヤーのインデックス
	constexpr int BULLET_PLAYER_INDEX = 1;		// 弾プレイヤーのインデックス
	constexpr int INTERIOR_PLAYER_INDEX = 2;	// 裏プレイヤーのインデックス
}

AbilityActionHint::AbilityActionHint()
{
	_iHandleAbsorbAction			= ResourceServer::GetInstance()->GetHandle("SkillAbsorb");
	_iHandleAoeAction				= ResourceServer::GetInstance()->GetHandle("SkillAoe");
	_iHandlePunchAction				= ResourceServer::GetInstance()->GetHandle("SkillPunch");
	_iHandlePiercingBulletAction	= ResourceServer::GetInstance()->GetHandle("SkillPiercingBullet");
	_iHandleNormalBulletAction		= ResourceServer::GetInstance()->GetHandle("SkillNormalBullet");
}

AbilityActionHint::~AbilityActionHint()
{
	// 画像の開放
	DeleteGraph(_iHandleAbsorbAction);
	DeleteGraph(_iHandleAoeAction);
	DeleteGraph(_iHandlePunchAction);	
	DeleteGraph(_iHandlePiercingBulletAction);
	DeleteGraph(_iHandleNormalBulletAction);
}

bool AbilityActionHint::Initialize()
{
	return true;
}

bool AbilityActionHint::Terminate()
{
	return true;
}

bool AbilityActionHint::Process()
{
	return true;
}

bool AbilityActionHint::Render()
{
	return true;
}

void AbilityActionHint::ActionHintRender(ABILITY_TYPE abilityType, float selectGraphSenterX, float secondSelectGraphCenterX)
{
	// 画像のサイズを取得
	int graphW, graphH;
	GetGraphSize(_iHandleAbsorbAction, &graphW, &graphH);

	float graphCenterX = static_cast<float>(graphW / 2);	// 画像の中心X座標

	// 能力タイプに応じてアクションヒントを描画
	switch(abilityType)
	{
		case ABILITY_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			/* 表プレイヤーのアクションヒントを描画 */

			// 吸収攻撃アクションヒントを描画
			DrawGraph(selectGraphSenterX - graphCenterX, RenderConfig::DRAW_Y, _iHandleAbsorbAction, TRUE);

			break;
		}
			
		case ABILITY_TYPE::BULLET_PLAYER: // 弾発射プレイヤー
		{
			/* 弾プレイヤーのアクションヒントを描画 */

			// 通常弾アクションヒントを描画
			DrawGraph(selectGraphSenterX - graphCenterX, RenderConfig::DRAW_Y, _iHandleNormalBulletAction, TRUE);

			// 貫通弾アクションヒントを描画
			DrawGraph(secondSelectGraphCenterX - graphCenterX, RenderConfig::DRAW_Y, _iHandlePiercingBulletAction, TRUE);

			break;
		}

		case ABILITY_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			/* 裏プレイヤーのアクションヒントを描画 */

			// 範囲攻撃アクションヒントを描画
			//DrawGraph(selectGraphSenterX - graphCenterX, RenderConfig::DRAW_Y, _iHandleAoeAction, TRUE);

			// 打撃攻撃アクションヒントを描画
			DrawGraph(secondSelectGraphCenterX - graphCenterX, RenderConfig::DRAW_Y, _iHandlePunchAction, TRUE);

			break;
		}

		default:
			break;
	}

	//// 能力タイプに応じてアクションヒントを描画
	//switch(selectedIndex)
	//{
	//	case ABILITY_TYPE::SURFACE_PLAYER: // 表プレイヤー
	//	{
	//		// 表プレイヤーのアクションヒントを描画
	//		DrawGraph(RenderConfig::DRAW_X, RenderConfig::DRAW_Y, _iHandleAbsorbAction, TRUE);

	//		break;
	//	}
	//		
	//	case ABILITY_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
	//	{
	//		// 裏プレイヤーのアクションヒントを描画
	//		DrawGraph(RenderConfig::DRAW_X, RenderConfig::DRAW_Y, _iHandleAoeAction, TRUE);

	//		break;
	//	}

	//	case ABILITY_TYPE::BULLET_PLAYER: // 弾プレイヤー
	//	{
	//		// 弾プレイヤーのアクションヒントを描画
	//		DrawGraph(RenderConfig::DRAW_X, RenderConfig::DRAW_Y, _iHandlePunchAction, TRUE);

	//		break;
	//	}

	//	default:
	//		break;
	//}
}
