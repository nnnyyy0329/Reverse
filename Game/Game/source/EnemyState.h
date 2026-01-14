#pragma once
#include "appframe.h"
#include <memory>

class Enemy;

struct EnemyParam
{
	float moveSpeed = 2.0f;// 敵の基本移動速度

	float visionRange = 0.0f;// 敵の索敵距離
	float visionAngle = 45.0f;// 敵の視界の角度(半分)
	float visionCos = 0.0f;// 判定で使うcos値

	float attackRange = 0.0f;// これ以内なら攻撃する距離
	float chaseLimitRange = 0.0f;// これ以上離れたら追跡をやめる距離

	float moveRadius = 0.0f;// 徘徊する範囲の半径(初期位置からの距離)

	float attackInterval = 0.0f;// 攻撃間隔

	float turnSpeed = 1.0f;// 旋回速度(度 / フレーム)

	float idleTime = 0.0f;// 待機時間
	float moveTime = 0.0f;// 自動移動時間
	float detectTime = 0.0f;// 発見硬直
	float attackTime = 0.0f;// 攻撃時間
};

class EnemyState
{
public:
	virtual ~EnemyState() = default;

	virtual void Enter(Enemy* owner) {}// その状態の切り替わった直後に一度だけ呼ばれる
	virtual std::shared_ptr<EnemyState> Update(Enemy* owner) = 0;// 毎フレーム呼ばれる。状態遷移する場合は新しい状態を返す。継続する場合はnullptrを返す
	virtual void Exit(Enemy* owner) {}// 別の状態に切り替わる直前に一度だけ呼ばれる。後始末はここで

	virtual const char* GetName() const { return "None"; }// デバッグ用に状態名を取得

	virtual bool IsChasing() const { return false; }// 追跡状態かどうか

	float _fTimer = 0.0f;
};
