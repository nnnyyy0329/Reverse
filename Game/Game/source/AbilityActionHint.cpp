#include "AbilityActionHint.h"

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
	// アクションヒントの描画
	ActionHintRender();

	return true;
}

void AbilityActionHint::ActionHintRender()
{

}
