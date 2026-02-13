#pragma once
#include "appframe.h"
#include "AttackBase.h"

struct PlayerAttackMovement
{
	VECTOR moveDir;     // 移動方向
	float moveDistance; // 移動距離
	float moveSpeed;    // 移動速度
	float decayRate;    // 減衰率
	bool canMove;       // 移動可能フラグ
};

// プレイヤー攻撃システム
class PlayerAttackSystem : public AttackBase
{
public:
	PlayerAttackSystem();
	virtual ~PlayerAttackSystem();

	// 移動計算処理
	virtual AttackMovement CalculateMovement(ATTACK_STATE state, std::shared_ptr<CharaBase> owner) override;

protected:
	PlayerAttackMovement _attackMovement;	// 攻撃移動情報

};

