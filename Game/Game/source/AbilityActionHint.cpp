#include "AbilityActionHint.h"

// 能力入力ヒント表示用の定数エイリアス
namespace AHC = AbilityHintConfig;

AbilityActionHint::AbilityActionHint()
{
	_iHandleAbsorbAction			= ResourceServer::GetInstance()->GetHandle("SkillAbsorb");
	_iHandleAoeAction				= ResourceServer::GetInstance()->GetHandle("SkillAoe");
	_iHandlePunchAction				= ResourceServer::GetInstance()->GetHandle("SkillPunch");
	_iHandlePiercingBulletAction	= ResourceServer::GetInstance()->GetHandle("SkillPiercingBullet");
	_iHandleNormalBulletAction		= ResourceServer::GetInstance()->GetHandle("SkillNormalBullet");

	_iHandleDodgeAction				= ResourceServer::GetInstance()->GetHandle("ActionHintDodge");
	_iHandleTransCancelAction		= ResourceServer::GetInstance()->GetHandle("ActionHintTransCancel");
}

AbilityActionHint::~AbilityActionHint()
{
	// デストラクタ
	Terminate();
}

bool AbilityActionHint::Initialize()
{
	return true;
}

bool AbilityActionHint::Terminate()
{
	// 画像の開放

	DeleteGraph(_iHandleAbsorbAction);
	DeleteGraph(_iHandleAoeAction);
	DeleteGraph(_iHandlePunchAction);
	DeleteGraph(_iHandlePiercingBulletAction);
	DeleteGraph(_iHandleNormalBulletAction);
	DeleteGraph(_iHandleDodgeAction);
	DeleteGraph(_iHandleTransCancelAction);

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

void AbilityActionHint::AbilityInputHintRender(ABILITY_TYPE abilityType, float selectGraphSenterX, float secondSelectGraphCenterX)
{
	// 画像のサイズを取得
	int graphW, graphH;
	GetGraphSize(_iHandleAbsorbAction, &graphW, &graphH);

	// 画像の中心X座標
	float graphCenterX = static_cast<float>(graphW / 2);	

	// 能力タイプに応じてアクションヒントを描画
	switch(abilityType)
	{
		case ABILITY_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			/* 表プレイヤーのアクションヒントを描画 */

			// 吸収攻撃アクションヒントを描画
			DrawGraph(static_cast<int>(selectGraphSenterX - graphCenterX), AHC::DRAW_Y, _iHandleAbsorbAction, TRUE);

			break;
		}
			
		case ABILITY_TYPE::BULLET_PLAYER: // 弾発射プレイヤー
		{
			/* 弾プレイヤーのアクションヒントを描画 */

			// 通常弾アクションヒントを描画
			DrawGraph(static_cast<int>(selectGraphSenterX - graphCenterX), AHC::DRAW_Y, _iHandleNormalBulletAction, TRUE);

			// 貫通弾アクションヒントを描画
			DrawGraph(static_cast<int>(secondSelectGraphCenterX - graphCenterX), AHC::DRAW_Y, _iHandlePiercingBulletAction, TRUE);

			break;
		}

		case ABILITY_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			/* 裏プレイヤーのアクションヒントを描画 */

			// 範囲攻撃アクションヒントを描画
			//DrawGraph(static_cast<int>(selectGraphSenterX - graphCenterX), AHC::DRAW_Y, _iHandleAoeAction, TRUE);

			// 打撃攻撃アクションヒントを描画
			DrawGraph(static_cast<int>(secondSelectGraphCenterX - graphCenterX), AHC::DRAW_Y, _iHandlePunchAction, TRUE);

			break;
		}

		default:
			break;
	}
}

void AbilityActionHint::ActionInputHintRender()
{

}
