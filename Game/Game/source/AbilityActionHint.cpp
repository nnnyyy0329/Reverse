#include "AbilityActionHint.h"

// 描画位置設定用の名前空間
namespace RenderConfig
{
	constexpr int DRAW_X = 1500;	// 描画位置X
	constexpr int DRAW_Y = 550;		// 描画位置Y
	constexpr int OFFSET_X = 100;	// 描画位置オフセットX
	constexpr int OFFSET_Y = 0;		// 描画位置オフセットY
}

// アクションヒントの選択インデックス設定用の名前空間
namespace SelectedIndexConfig
{
	constexpr int SURFACE_PLAYER_INDEX = 0;		// 表プレイヤーのインデックス
	constexpr int INTERIOR_PLAYER_INDEX = 1;	// 裏プレイヤーのインデックス
	constexpr int BULLET_PLAYER_INDEX = 2;		// 弾プレイヤーのインデックス
}

AbilityActionHint::AbilityActionHint()
{
	_iHandleAbsorbAction = ResourceServer::GetInstance()->GetHandle("SkillAbsorb");
	_iHandleAoeAction = ResourceServer::GetInstance()->GetHandle("SkillAoe");
	_iHandlePunchAction = ResourceServer::GetInstance()->GetHandle("SkillPunch");
	_iHandlePiercingBulletAction = ResourceServer::GetInstance()->GetHandle("SkillPiercingBullet");
	_iHandleNormalBulletAction = ResourceServer::GetInstance()->GetHandle("SkillNormalBullet");
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

void AbilityActionHint::ActionHintRender(int selectedIndex, ABILITY_TYPE abilityType)
{
	// 画像のサイズを取得
	int graphW, graphH;
	GetGraphSize(_iHandleAbsorbAction, &graphW, &graphH);

	int drawX = RenderConfig::DRAW_X;							// 描画位置X(ラップ)
	int drawY = RenderConfig::DRAW_Y;							// 描画位置Y(ラップ)
	int offsetDrawX = drawX + graphW + RenderConfig::DRAW_X;	// オフセット適応後の描画位置X
	int offsetDrawY = drawY + RenderConfig::DRAW_Y;				// オフセット適応後の描画位置Y

	// 能力タイプに応じてアクションヒントを描画
	switch(selectedIndex)
	{
		case SelectedIndexConfig::SURFACE_PLAYER_INDEX: // 表プレイヤー
		{
			/* 表プレイヤーのアクションヒントを描画 */

			// 吸収攻撃アクションヒントを描画
			DrawGraph(drawX, drawY, _iHandleAbsorbAction, TRUE);

			break;
		}
			
		case SelectedIndexConfig::BULLET_PLAYER_INDEX: // 弾発射プレイヤー
		{
			/* 弾プレイヤーのアクションヒントを描画 */

			// 貫通弾アクションヒントを描画
			DrawGraph(drawX, drawY, _iHandlePiercingBulletAction, TRUE);

			// 通常弾アクションヒントを描画
			DrawGraph(offsetDrawX, offsetDrawY, _iHandleNormalBulletAction, TRUE);

			break;
		}

		case SelectedIndexConfig::INTERIOR_PLAYER_INDEX: // 裏プレイヤー
		{
			/* 裏プレイヤーのアクションヒントを描画 */

			// 範囲攻撃アクションヒントを描画
			DrawGraph(drawX, drawY, _iHandleAoeAction, TRUE);

			// 打撃攻撃アクションヒントを描画
			DrawGraph(offsetDrawX, offsetDrawY, _iHandlePunchAction, TRUE);

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
