#pragma once
#include "EnemyState.h"

namespace Common
{
	/*
	* 共通で使うステート
	*/

	// 被ダメージ
	class Damage : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Damage"; }
	};

	// 死亡
	class Dead : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Dead"; }
	};

	// スタン
	class Stun : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Stun"; }
	};

	// ダウン
	class Down : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Down"; }
	};
}

