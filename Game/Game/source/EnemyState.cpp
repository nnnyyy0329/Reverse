#include "EnemyState.h"
#include "Enemy.h"

float EnemyState::CalcRandomRangeTime(float fBaseTime, float fRange)
{
	if (fRange < 0.0f) { fRange = 0.0f; }

	// RandomRangeで[-fRange, +fRange]のランダムオフセットを生成
	float fOffset = (fRange > 0.0f) ? mymath::RandomRange(-fRange, fRange) : 0.0f;

	float fResult = fBaseTime + fOffset;
	if (fResult < 0.0f) { fResult = 0.0f; }

	return fResult;
}

// ターゲット情報を取得
TargetInfo EnemyState::GetTargetInfo(Enemy* owner)
{
	TargetInfo info;
	info.target = owner->GetTarget();
	info.bExist = (info.target != nullptr);

	if (info.bExist)
	{
		// ターゲットへのベクトル計算
		info.vToTarget = VSub(info.target->GetPos(), owner->GetPos());
		info.fDist = VSize(info.vToTarget);

		// 方向ベクトル計算(FlattenVectorでY除去+正規化)
		info.vDir = mymath::FlattenVector(info.vToTarget);
	}
	else
	{
		info.vToTarget = VGet(0.0f, 0.0f, 0.0f);
		info.vDir = VGet(0.0f, 0.0f, 0.0f);
		info.fDist = 0.0f;
	}

	return info;
}

// ターゲット方向へ回転
void EnemyState::RotateToTarget(Enemy* owner, const VECTOR& vDir, float fRotSpeed)
{
	owner->SmoothRotateTo(vDir, fRotSpeed);
}

// ターゲット方向へ移動
void EnemyState::MoveToTarget(Enemy* owner, const VECTOR& vDir, float fSpeed)
{
	VECTOR vMove = VScale(vDir, fSpeed);
	owner->SetMove(vMove);
}

// 移動停止
void EnemyState::StopMove(Enemy* owner)
{
	owner->SetMove(VGet(0.0f, 0.0f, 0.0f));
}