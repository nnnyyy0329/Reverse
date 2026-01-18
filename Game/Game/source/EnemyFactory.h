#pragma once
#include "Enemy.h"
#include "StateMelee.h"
#include "StateRanged.h"
#include <memory>

namespace {
	constexpr auto DEFAULT_ENEMY_SPEED = 2.0f;// 敵の移動速度
	constexpr auto DEFAULT_ENEMY_MAX_LIFE = 100.0f;// 敵の最大体力

	// Melee
	constexpr auto MELEE_VISION_RANGE = 250.0f;// 索敵距離
	constexpr auto MELEE_ATTACK_RANGE = 50.0f;// これ以内なら攻撃する距離
	constexpr auto MELEE_CHASE_LIMIT_RANGE = 800.0f;// これ以上離れたら追跡をやめる距離
	constexpr auto MELEE_MOVE_RADIUS = 1000.0f;// 徘徊する範囲の半径(初期位置からの距離)
	constexpr auto MELEE_IDLE_TIME = 120.0f;// 待機時間
	constexpr auto MELEE_MOVE_TIME = 180.0f;// 自動移動時間
	constexpr auto MELEE_DETECT_TIME = 30.0f;// 発見硬直
	constexpr auto MELEE_ATTACK_TIME = 60.0f;// 攻撃時間

	// Ranged
	constexpr auto RANGED_VISION_RANGE = 800.0f;// 索敵距離
	constexpr auto RANGED_MOVEBACK_RANGE = 300.0f;// これより近づいたら後退する
	constexpr auto RANGED_ATTACK_INTERVAL = 120.0f;// 攻撃間隔
	constexpr auto RANGED_DETECT_TIME = 30.0f;// 発見硬直
}

// 敵の種類
enum class EnemyType
{
	MELEE,// 近接
	RANGED,// 遠距離
	_EOT_,
};

class EnemyFactory
{
public:
	// 敵の種類、座標を指定して敵を作成する
	static std::shared_ptr<Enemy> CreateEnemy(EnemyType type, VECTOR pos) 
	{
		auto enemy = std::make_shared<Enemy>();

		enemy->SetPos(pos);// 座標を設定
		enemy->Initialize();// 座標の設定後に呼ぶ

		EnemyParam param;

		switch (type) {
		case EnemyType::MELEE:// 近接型
				param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
				param.fVisionRange = MELEE_VISION_RANGE;
				param.fAttackRange = MELEE_ATTACK_RANGE;
				param.fChaseLimitRange = MELEE_CHASE_LIMIT_RANGE;
				param.fMoveRadius = MELEE_MOVE_RADIUS;
				param.fIdleTime = MELEE_IDLE_TIME;
				param.fMoveTime = MELEE_MOVE_TIME;
				param.fDetectTime = MELEE_DETECT_TIME;
				param.fAttackTime = MELEE_ATTACK_TIME;
				param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;
				enemy->SetEnemyParam(param);// パラメータ設定

				// ハンドラの中身を設定
				enemy->SetRecoveryHandler([](Enemy* e) -> std::shared_ptr<EnemyState> {
					// ターゲットが見えていれば追跡
					if (e->IsTargetVisible(e->GetTarget())) {
						return std::make_shared<Melee::Chase>();
					}
					// 見えていなければ待機
					return std::make_shared<Melee::Idle>();
				});

				enemy->ChangeState(std::make_shared<Melee::Idle>());// 初期状態設定
				break;
		case EnemyType::RANGED:// 遠距離型
				param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
				param.fVisionRange = RANGED_VISION_RANGE;
				param.fAttackRange = RANGED_MOVEBACK_RANGE;
				param.fAttackInterval = RANGED_ATTACK_INTERVAL;
				param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;
				enemy->SetEnemyParam(param);// パラメータ設定

				// ハンドラの中身を設定
				enemy->SetRecoveryHandler([](Enemy* e) -> std::shared_ptr<EnemyState> {
					// ターゲットが見えていれば攻撃
					if (e->IsTargetVisible(e->GetTarget())) {
						return std::make_shared<Ranged::Attack>();
					}
					// 見えていなければ待機
					return std::make_shared<Ranged::Idle>();
					});

				enemy->ChangeState(std::make_shared<Ranged::Idle>());// 初期状態設定
				break;
		}

		return enemy;// 作成した敵を返す
	}
};
