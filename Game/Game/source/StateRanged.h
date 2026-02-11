#pragma once
#include "EnemyState.h"

namespace Ranged
{
	// 円形視界判定(距離のみチェック)
	bool IsTargetVisible(Enemy* owner);

	// 待機
	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Idle"; }// 名前を返す(デバッグ用)
		void UpdateSearch(Enemy* owner) override;
	};





	// 徘徊(その場で周囲を見渡す)
	class Wander : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Wander"; }
		void UpdateSearch(Enemy* owner) override;

	private:
		VECTOR _vTargetDir;// 目標方向ベクトル
		float _fLookDuration;// 視線停止時間
	};
	





	// 発見
	class Notice : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Notice"; }
	};





	// 接近
	class Approach : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Approach"; }
		bool IsChasing() const override { return true; }
	};





	// 射撃ステート開始
	class ShotStart : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ShotStart"; }
		bool IsChasing() const override { return true; }
	};

	// 射撃溜め
	class ShotCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ShotCharge"; }
		bool CanChangeState() override { return false; }
	};

	// 射撃実行
	class ShotExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ShotExecute"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bHasShot;
	};

	// 射撃後隙
	class ShotRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ShotRecovery"; }
	};

	// 射撃間隔
	class ShotInterval : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ShotInterval"; }
	};





	// 帰還
	class ReturnHome : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:ReturnHome"; }
		void UpdateSearch(Enemy* owner) override;
	};
}
