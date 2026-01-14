#pragma once
#include "Enemy.h"
#include "StateMelee.h"
#include "StateRanged.h"
#include <memory>

namespace {
	constexpr auto DEFAULT_ENEMY_SPEED = 2.0f;// “G‚ÌˆÚ“®‘¬“x

	// Melee
	constexpr auto MELEE_VISION_RANGE = 250.0f;// õ“G‹——£
	constexpr auto MELEE_ATTACK_RANGE = 50.0f;// ‚±‚êˆÈ“à‚È‚çUŒ‚‚·‚é‹——£
	constexpr auto MELEE_CHASE_LIMIT_RANGE = 800.0f;// ‚±‚êˆÈã—£‚ê‚½‚ç’ÇÕ‚ğ‚â‚ß‚é‹——£
	constexpr auto MELEE_MOVE_RADIUS = 1000.0f;// œpœj‚·‚é”ÍˆÍ‚Ì”¼Œa(‰ŠúˆÊ’u‚©‚ç‚Ì‹——£)
	constexpr auto MELEE_IDLE_TIME = 120.0f;// ‘Ò‹@ŠÔ
	constexpr auto MELEE_MOVE_TIME = 180.0f;// ©“®ˆÚ“®ŠÔ
	constexpr auto MELEE_DETECT_TIME = 30.0f;// ”­Œ©d’¼
	constexpr auto MELEE_ATTACK_TIME = 60.0f;// UŒ‚ŠÔ

	// Ranged
	constexpr auto RANGED_VISION_RANGE = 800.0f;// õ“G‹——£
	constexpr auto RANGED_MOVEBACK_RANGE = 300.0f;// ‚±‚ê‚æ‚è‹ß‚Ã‚¢‚½‚çŒã‘Ş‚·‚é
	constexpr auto RANGED_ATTACK_INTERVAL = 120.0f;// UŒ‚ŠÔŠu
	constexpr auto RANGED_DETECT_TIME = 30.0f;// ”­Œ©d’¼
}

enum class EnemyType
{
	MELEE,
	RANGED,
	_EOT_,
};

class EnemyFactory
{
public:
	// “G‚Ìí—ŞAÀ•W‚ğw’è‚µ‚Ä“G‚ğì¬‚·‚é
	static std::shared_ptr<Enemy> CreateEnemy(EnemyType type, VECTOR pos) 
	{
		auto enemy = std::make_shared<Enemy>();

		enemy->SetPos(pos);// À•W‚ğİ’è
		enemy->Initialize();// À•W‚Ìİ’èŒã‚ÉŒÄ‚Ô

		EnemyParam param;

		switch (type) {
		case EnemyType::MELEE:// ‹ßÚŒ^
				param.moveSpeed = DEFAULT_ENEMY_SPEED;
				param.visionRange = MELEE_VISION_RANGE;
				param.attackRange = MELEE_ATTACK_RANGE;
				param.chaseLimitRange = MELEE_CHASE_LIMIT_RANGE;
				param.moveRadius = MELEE_MOVE_RADIUS;
				param.idleTime = MELEE_IDLE_TIME;
				param.moveTime = MELEE_MOVE_TIME;
				param.detectTime = MELEE_DETECT_TIME;
				param.attackTime = MELEE_ATTACK_TIME;
				enemy->SetEnemyParam(param);// ƒpƒ‰ƒ[ƒ^İ’è
				enemy->ChangeState(std::make_shared<Melee::Idle>());// ‰Šúó‘Ôİ’è
				break;
		case EnemyType::RANGED:// ‰“‹——£Œ^
				param.moveSpeed = DEFAULT_ENEMY_SPEED;
				param.visionRange = RANGED_VISION_RANGE;
				param.attackRange = RANGED_MOVEBACK_RANGE;
				param.attackInterval = RANGED_ATTACK_INTERVAL;
				enemy->SetEnemyParam(param);// ƒpƒ‰ƒ[ƒ^İ’è
				enemy->ChangeState(std::make_shared<Ranged::Idle>());// ‰Šúó‘Ôİ’è
				break;
		}

		return enemy;// ì¬‚µ‚½“G‚ğ•Ô‚·
	}
};
