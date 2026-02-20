#include "EnemyState.h"
#include "Enemy.h"

float EnemyState::CalcRandomRangeTime(float fBaseTime, float fRange)
{
	if (fRange < 0.0f) { fRange = 0.0f; }

	// 中央を0とするため、範囲の2倍を整数で計算
	int iRange = static_cast<int>(fRange * 2.0f);

	// 0 ~ iRange から fRange を引いて -fRange ~ +fRange にする
	float fOffset = 0.0f;
	if (fRange > 0)
	{
		fOffset = static_cast<float>(GetRand(iRange)) - fRange;
	}

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

		// 方向ベクトル計算
		if (info.fDist > 0.0f)
		{
			info.vDir = VScale(info.vToTarget, 1.0f / info.fDist);
		}
		else
		{
			info.vDir = VGet(0.0f, 0.0f, 0.0f);
		}
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