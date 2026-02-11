#pragma once
#include "Enemy.h"
#include "StateMelee.h"
#include "StateRanged.h"
#include "StateTank.h"
#include <memory>

namespace 
{
	constexpr auto DEFAULT_ENEMY_SPEED = 2.0f;// 敵の移動速度
	constexpr auto DEFAULT_ENEMY_MAX_LIFE = 100.0f;// 敵の最大体力

	// Melee
	constexpr auto MELEE_VISION_RANGE = 250.0f;// 索敵距離
	constexpr auto MELEE_VISION_ANGLE = 180.0f;// 円での索敵
	constexpr auto MELEE_ATTACK_RANGE = 120.0f;// これ以内なら攻撃する距離
	constexpr auto MELEE_CHASE_LIMIT_RANGE = 600.0f;// これ以上離れたら接近をやめる距離
	constexpr auto MELEE_MOVE_RADIUS = 800.0f;// 徘徊する範囲の半径(初期位置からの距離)
	constexpr auto MELEE_IDLE_TIME = 120.0f;// 待機時間
	constexpr auto MELEE_MOVE_TIME = 180.0f;// 徘徊時間
	constexpr auto MELEE_DETECT_TIME = 90.0f;// 発見硬直
	constexpr auto MELEE_ATTACK_TIME = 180.0f;// 攻撃時間

	// Ranged
	constexpr auto RANGED_VISION_RANGE = 800.0f;// 索敵距離(広め)
	constexpr auto RANGED_VISION_ANGLE = 180.0f;// 索敵角度(円形)
	constexpr auto RANGED_CHASE_LIMIT_RANGE = 1000.0f;// これ以上離れたら追跡をやめる距離
	constexpr auto RANGED_MOVE_RADIUS = 0.0f;// 徘徊範囲(移動しないため0)
	constexpr auto RANGED_IDLE_TIME = 120.0f;// 待機時間
	constexpr auto RANGED_MOVE_TIME = 0.0f;// 徘徊時間(使用しない)
	constexpr auto RANGED_DETECT_TIME = 60.0f;// 発見硬直

	// Tank
	constexpr auto TANK_VISION_RANGE = 300.0f;// 索敵距離
	constexpr auto TANK_VISION_ANGLE = 180.0f;// 索敵角度
	constexpr auto TANK_ATTACK_RANGE = 150.0f;// これ以内なら攻撃する距離
	constexpr auto TANK_CHASE_LIMIT_RANGE = 700.0f;// これ以上離れたら接近をやめる距離
	constexpr auto TANK_MOVE_RADIUS = 500.0f;// 徘徊する範囲の半径(初期位置からの距離)
	constexpr auto TANK_IDLE_TIME = 150.0f;;// 待機時間
	constexpr auto TANK_MOVE_TIME = 120.0f;// 徘徊時間
	constexpr auto TANK_DETECT_TIME = 120.0f;// 発見硬直
	constexpr auto TANK_MOVE_SPEED = 1.5f;// 移動速度
	constexpr auto TANK_DAMAGE_TO_DOWN = 5;// ダウンまでの被ダメージ回数
}

// 敵の種類
enum class EnemyType
{
	MELEE,// 近接
	RANGED,// 遠距離
	TANK,// タンク
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

		EnemyParam param;

		switch (type) 
		{
		case EnemyType::MELEE:// 近接型
			// モデル名を設定
			enemy->SetModelName("Melee");

				param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
				param.fVisionRange = MELEE_VISION_RANGE;
				param.fVisionAngle = MELEE_VISION_ANGLE;
				param.fAttackRange = MELEE_ATTACK_RANGE;
				param.fChaseLimitRange = MELEE_CHASE_LIMIT_RANGE;
				param.fMoveRadius = MELEE_MOVE_RADIUS;
				param.fIdleTime = MELEE_IDLE_TIME;
				param.fMoveTime = MELEE_MOVE_TIME;
				param.fDetectTime = MELEE_DETECT_TIME;
				param.fAttackTime = MELEE_ATTACK_TIME;
				param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;

				// 共通ステートのアニメーション名を設定
				param.animDamage = "enemy_damage00_00";
				param.animDead = "enemy_dead_00";
				param.animStun = "Melee_Stun";
				param.animDown = "enemy_damage01_00";

				enemy->SetEnemyParam(param);// パラメータ設定

				// ハンドラの中身を設定
				enemy->SetRecoveryHandler([](Enemy* e) -> std::shared_ptr<EnemyState> 
					{
					// ターゲットが存在すれば接近
					if (e->GetTarget()) 
					{
						return std::make_shared<Melee::Approach>();
					}
					// 見えていなければ待機
					return std::make_shared<Melee::Idle>();
				});
				break;

		case EnemyType::RANGED:// 遠距離型

			enemy->SetModelName("Ranged");

			param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
			param.fVisionRange = RANGED_VISION_RANGE;
			param.fVisionAngle = RANGED_VISION_ANGLE;
			param.fChaseLimitRange = RANGED_CHASE_LIMIT_RANGE;
			param.fMoveRadius = RANGED_MOVE_RADIUS;
			param.fIdleTime = RANGED_IDLE_TIME;
			param.fMoveTime = RANGED_MOVE_TIME;
			param.fDetectTime = RANGED_DETECT_TIME;
			param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;

			// 共通ステートのアニメーション名を設定
			param.animDamage = "enemy_damage00_00";
			param.animDead = "enemy_dead_00";
			param.animStun = "Ranged_Stun";
			param.animDown = "enemy_damage01_00";

			enemy->SetEnemyParam(param);

			// ハンドラの中身を設定
			enemy->SetRecoveryHandler([](Enemy* e) -> std::shared_ptr<EnemyState>
				{
					// ターゲットが存在すれば接近
					if (e->GetTarget())
					{
						return std::make_shared<Ranged::Approach>();
					}
					// 見えていなければ待機
					return std::make_shared<Ranged::Idle>();
				});
			break;

		case EnemyType::TANK:// タンク型

			enemy->SetModelName("Melee");

				param.fMoveSpeed = TANK_MOVE_SPEED;
				param.fVisionRange = TANK_VISION_RANGE;
				param.fVisionAngle = TANK_VISION_ANGLE;
				param.fAttackRange = TANK_ATTACK_RANGE;
				param.fChaseLimitRange = TANK_CHASE_LIMIT_RANGE;
				param.fMoveRadius = TANK_MOVE_RADIUS;
				param.fIdleTime = TANK_IDLE_TIME;
				param.fMoveTime = TANK_MOVE_TIME;
				param.fDetectTime = TANK_DETECT_TIME;
				param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;
				param.damageToDown = TANK_DAMAGE_TO_DOWN;
				enemy->SetEnemyParam(param);


				enemy->SetRecoveryHandler([](Enemy* e) -> std::shared_ptr<EnemyState> 
					{
					// ターゲットが存在すれば接近
					if (e->GetTarget())
					{
						return std::make_shared<Tank::Approach>();
					}
					// 見えていなければ待機
					return std::make_shared<Tank::Idle>();
				});
				break;
		}

		// 設定後に初期化を呼び出す
		enemy->Initialize();

		// 初期化後に初期状態を設定
		switch (type) 
		{
		case EnemyType::MELEE:
			enemy->ChangeState(std::make_shared<Melee::Idle>());
			break;
		case EnemyType::RANGED:
			enemy->ChangeState(std::make_shared<Ranged::Idle>());
			break;
		case EnemyType::TANK:
			enemy->ChangeState(std::make_shared<Tank::Idle>());
			break;
		}

		return enemy;// 作成した敵を返す
	}
};
