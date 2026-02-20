#pragma once
#include "appframe.h"
#include "AttackBase.h"

// 敵の種類ごとの攻撃コリジョン設定
struct EnemyAttackSettings
{
	COLLISION_TYPE colType = COLLISION_TYPE::CAPSULE;// コリジョンタイプ	

	// カプセル用:ローカルオフセット
	VECTOR vTopOffset = VGet(0.0f, 0.0f, 0.0f);
	VECTOR vBottomOffset = VGet(0.0f, 0.0f, 0.0f);
	float fRadius = 0.0f;

	// 時間関連
	float fDelay = 0.0f;// 発生遅延
	float fDuration = 0.0f;// 持続時間
	float fRecovery = 0.0f;// 後隙

	// ダメージ
	float fDamage = 0.0f;

	// AttackManager登録情報
	int ownerId = -1;
};
