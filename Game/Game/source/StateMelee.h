#pragma once
#include "EnemyState.h"

namespace Melee
{
	// 円形視界判定(距離のみチェック)
	bool IsTargetVisible(Enemy* owner);

	// 待機
	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Idle"; }// 名前を返す(デバッグ用)
		void UpdateSearch(Enemy* owner) override;
	};





	// 徘徊
	class Wander : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Wander"; }
		void UpdateSearch(Enemy* owner) override;
	};





	// 発見:見つけた瞬間の硬直
	class Notice : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Notice"; }
	};





	// 接近
	class Approach : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Approach"; }
		bool IsChasing() const override { return true; }// 接近状態である

	private:
		bool _bHasChecked;// 対峙チェックをしたか
	};





	// 攻撃
	class Attack : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Attack"; }
		bool CanChangeState() override;
	private:
		bool _bHasCollision;// 攻撃コリジョンが生成されたか
	};





	// 帰還
	class ReturnHome : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:ReturnHome"; }
		void UpdateSearch(Enemy* owner) override;
	};




	// 対峙
	class Confront : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Confront"; }

	private:
		float _fDuration;// 対峙時間
		int _direction;// 1:右回り, -1:左回り
	};
}

