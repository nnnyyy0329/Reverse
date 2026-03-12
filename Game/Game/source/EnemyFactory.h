#pragma once
#include "Enemy.h"
#include "StateMelee.h"
#include "StateRanged.h"
#include "StateTank.h"
#include "StateCommon.h"
#include <memory>

namespace 
{
	constexpr auto DEFAULT_ENEMY_SPEED = 2.0f;// 敵の移動速度
	constexpr auto DEFAULT_ENEMY_MAX_LIFE = 200.0f;// 敵の最大体力

	// Normal Melee
	constexpr auto NORMAL_VISION_RANGE = 250.0f;// 索敵距離
	constexpr auto NORMAL_VISION_ANGLE = 60.0f;// 円での索敵
	constexpr auto NORMAL_ATTACK_RANGE = 100.0f;// これ以内なら攻撃する距離
	constexpr auto NORMAL_CHASE_LIMIT_RANGE = 600.0f;// これ以上離れたら接近をやめる距離
	constexpr auto NORMAL_IDLE_TIME = 120.0f;// 待機時間
	constexpr auto NORMAL_MOVE_TIME = 180.0f;// 徘徊時間
	constexpr auto NORMAL_DETECT_TIME = 90.0f;// 発見硬直
	constexpr auto NORMAL_ATTACK_TIME = 180.0f;// 攻撃時間

	// Ranged
	constexpr auto RANGED_VISION_RANGE = 800.0f;// 索敵距離
	constexpr auto RANGED_VISION_ANGLE = 180.0f;// 索敵角度(円形)
	constexpr auto RANGED_CHASE_LIMIT_RANGE = 1000.0f;// これ以上離れたら追跡をやめる距離
	constexpr auto RANGED_MOVE_RADIUS = 0.0f;// 徘徊範囲(移動しないため0)
	constexpr auto RANGED_IDLE_TIME = 120.0f;// 待機時間
	constexpr auto RANGED_MOVE_TIME = 0.0f;// 徘徊時間(使用しない)
	constexpr auto RANGED_DETECT_TIME = 60.0f;// 発見硬直

	// Tank
	constexpr auto TANK_ATTACK_RANGE = 500.0f;// 攻撃ステートに入る距離
	constexpr auto TANK_ATTACK_LIMIT_RAMGE = 550.0f;// これ以上離れたら接近ステートへ
	constexpr auto TANK_DETECT_TIME = 120.0f;// 発見硬直
	constexpr auto TANK_MOVE_SPEED = 5.0f;// 移動速度
	constexpr auto TANK_IDLE_TIME = 120.0f;// 待機時間
}

// 敵の種類
enum class EnemyType
{
	NORMAL,// 通常
	MELEE,// 近接
	RANGED,// 遠距離
	TANK,// タンク
	_EOT_,
};

class EnemyFactory
{
public:
	// 敵の種類、座標を指定して敵を作成する
	static std::shared_ptr<Enemy> CreateEnemy(EnemyType type, VECTOR pos, VECTOR rot, bool bTransToWander = true) 
	{
		auto enemy = std::make_shared<Enemy>();

		enemy->SetPos(pos);// 座標を設定

		EnemyParam param;

		switch (type) 
		{
		case EnemyType::NORMAL:// 通常型
		case EnemyType::MELEE:// 近接型
			// モデル名を設定
			enemy->SetModelName("Melee");

			param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
			param.fVisionRange = NORMAL_VISION_RANGE;
			param.fVisionAngle = NORMAL_VISION_ANGLE;
			param.fAttackRange = NORMAL_ATTACK_RANGE;
			param.fChaseLimitRange = NORMAL_CHASE_LIMIT_RANGE;
			param.fIdleTime = NORMAL_IDLE_TIME;
			param.fMoveTime = NORMAL_MOVE_TIME;
			param.fDetectTime = NORMAL_DETECT_TIME;
			param.fAttackTime = NORMAL_ATTACK_TIME;
			param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;
			param.bTransToWander = bTransToWander;

			// 共通ステートのアニメーション名を設定
			param.animDamage = "enemy_damage_00";
			param.animDead = "enemy_dead_00";
			param.animDown = "enemy_damage_01";

			enemy->SetEnemyParam(param);// パラメータ設定

			// 被ダメ後の遷移先を決定
			enemy->SetAfterDamageStateSelector([](Enemy* e, int comboCnt)->std::shared_ptr<EnemyState>
			{
				if (e->GetTarget())
				{
					return std::make_shared<Normal::Approach>();
				}

				return std::make_shared<Normal::Idle>();
			});

			// ダウン後の遷移先を決定
			enemy->SetAfterDownStateSelector([](Enemy* e)->std::shared_ptr<EnemyState>
			{
				if (e->IsDead())
				{
					return std::make_shared<Common::Dead>();
				}

				if (e->GetTarget())
				{
					return std::make_shared<Normal::Approach>();
				}

				return std::make_shared<Normal::Idle>();
			});

		break;

		case EnemyType::RANGED:// 遠距離型

			enemy->SetModelName("Ranged");

			param.fMoveSpeed = DEFAULT_ENEMY_SPEED;
			param.fVisionRange = RANGED_VISION_RANGE;
			param.fVisionAngle = RANGED_VISION_ANGLE;
			param.fChaseLimitRange = RANGED_CHASE_LIMIT_RANGE;
			param.fIdleTime = RANGED_IDLE_TIME;
			param.fMoveTime = RANGED_MOVE_TIME;
			param.fDetectTime = RANGED_DETECT_TIME;
			param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;

			// 共通ステートのアニメーション名を設定
			param.animDamage = "Senemy_damage_00";
			param.animDead = "Senemy_dead_00";
			param.animDown = "Senemy_damage_01";

			enemy->SetEnemyParam(param);

			// 被ダメ後の遷移先を決定
			enemy->SetAfterDamageStateSelector([](Enemy* e, int comboCnt)->std::shared_ptr<EnemyState>
			{
				if (e->GetTarget())
				{
					return std::make_shared<Ranged::Approach>();
				}

				return std::make_shared<Ranged::Idle>();
			});

			// 被ダメ後の遷移先を決定
			enemy->SetAfterDownStateSelector([](Enemy* e)->std::shared_ptr<EnemyState>
			{
				if (e->IsDead())
				{
					return std::make_shared<Common::Dead>();
				}

				if (e->GetTarget())
				{
					return std::make_shared<Ranged::Approach>();
				}

				return std::make_shared<Ranged::Idle>();
			});

		break;

		case EnemyType::TANK:// タンク型

			enemy->SetModelName("Melee");

			param.fMoveSpeed = TANK_MOVE_SPEED;
			param.fAttackRange = TANK_ATTACK_RANGE;
			param.fChaseLimitRange = TANK_ATTACK_LIMIT_RAMGE;
			param.fDetectTime = TANK_DETECT_TIME;
			param.fMaxLife = DEFAULT_ENEMY_MAX_LIFE;
			param.fIdleTime = TANK_IDLE_TIME;

			// 共通ステートのアニメーション名を設定
			param.animDamage = "enemy_damage_00";
			param.animDead = "enemy_dead_00";
			param.animDown = "enemy_damage_01";

			enemy->SetEnemyParam(param);

			// 被ダメ後の遷移先を決定
			enemy->SetAfterDamageStateSelector([](Enemy* e, int comboCnt)->std::shared_ptr<EnemyState>
			{
				if (e->GetTarget())
				{
					return std::make_shared<Tank::Approach>();
				}

				return std::make_shared<Tank::Idle>();
			});

			// ダウン後の遷移先を決定
			enemy->SetAfterDownStateSelector([](Enemy* e)->std::shared_ptr<EnemyState>
			{
				if (e->IsDead())
				{
					return std::make_shared<Common::Dead>();
				}

				if (e->GetTarget())
				{
					return std::make_shared<Tank::Approach>();
				}

				return std::make_shared<Tank::Idle>();
			});

		break;
		}

		// 設定後に初期化を呼び出す
		enemy->Initialize();

		// 初期化後に初期状態を設定
		switch (type) 
		{
			case EnemyType::NORMAL:
			enemy->ChangeState(std::make_unique<Normal::Idle>());
			break;
		case EnemyType::MELEE:
			enemy->ChangeState(std::make_unique<Melee::Idle>());
			break;
		case EnemyType::RANGED:
			enemy->ChangeState(std::make_unique<Ranged::Idle>());
			break;
		case EnemyType::TANK:
			enemy->ChangeState(std::make_unique<Tank::Idle>());
			break;
		}

		VECTOR vDir = VGet(sinf(rot.y), 0.0f, cosf(rot.y));
		enemy->SetDir(vDir);
		enemy->SetHomeDir(vDir);

		return enemy;// 作成した敵を返す
	}
};
