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
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::DAMAGE; }// 優先度高め
	};

	// 死亡
	class Dead : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Dead"; }
		bool CanChangeState() override { return false; }// 死亡中はステート変更不可
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::TOP; }// 最優先
	};

	// スタン
	class Stun : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Stun"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::TOP; }// 最優先
	};

	// ダウン
	class Down : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Common:Down"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::TOP; }// 最優先
	};
}

