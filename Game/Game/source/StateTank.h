#pragma once
#include "EnemyState.h"

namespace Tank
{
	// 待機
	class Idle : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:Idle"; }// 名前を返す(デバッグ用)
		void UpdateSearch(Enemy* owner) override;
	};





	// 自動移動
	class Move : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:Move"; }
		void UpdateSearch(Enemy* owner) override;
	};





	// 発見
	class Detect : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:Detect"; }
	};





	// 追跡
	class Chase : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:Chase"; }
		bool IsChasing() const override { return true; }// 追跡状態である
	};





	// 一段目攻撃前の予備動作
	class FirstAttackPrepare : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:FirstAttackPrepare"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }
	};





	// 一段目攻撃の実行
	class FirstAttackExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Tank:FirstAttackExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bAttackStarted = false;
		float _fApproachSpeed;// 攻撃時の接近速度
	};





	// 一段目攻撃の後隙
	class FirstAttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Tank:FirstAttackRecovery"; }
		bool CanChangeState() override { return _bIsCompleted; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bIsCompleted = false;// 後隙完了フラグ
	};





	// 二段目攻撃前の予備動作
	class SecondAttackPrepare : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:SecondAttackPrepare"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }
	};





	// 二段目攻撃の実行
	class SecondAttackExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Tank:SecondAttackExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }	

	private:
		bool _bAttackStarted = false;
		float _fApproachSpeed;// 攻撃時の接近速度
	};





	// 二段目攻撃の後隙
	class SecondAttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Tank:SecondAttackRecovery"; }
		bool CanChangeState() override { return _bIsCompleted; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bIsCompleted = false;// 後隙完了フラグ
	};





	// 攻撃判定ステート
	class Attack : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Tank:Attack"; }
	};





	// 帰還
	class ReturnHome : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Tank:ReturnHome"; }
		void UpdateSearch(Enemy* owner) override;
	};
}