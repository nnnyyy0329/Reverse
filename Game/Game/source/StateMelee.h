#pragma once
#include "EnemyState.h"

namespace Melee
{
	/*
	* Idle:待機
	* ↑↓時間経過
	* Move:自動移動
	* ↓プレイヤー索敵(TryDetectPlayer)
	* Detect:発見
	* ↓時間経過
	* Chase:追跡(距離が離れすぎたらIdleへ)
	* ↑↓プレイヤーが攻撃射程内に入ったら
	* Attack:攻撃(Chaseに戻り距離を再確認)
	*/

	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Idle"; }// 名前を返す(デバッグ用)
	};

	class Move : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Move"; }
	};

	// 発見:見つけた瞬間の硬直
	class Detect : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Detect"; }
	};

	class Chase : public EnemyState
	{
		public:
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Chase"; }
		bool IsChasing() const override { return true; }// 追跡状態である
	};

	class Attack : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Attack"; }
	};
}

