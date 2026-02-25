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
		const char* GetName() override { return "Ranged:Idle"; }// 名前を返す(デバッグ用)
		void UpdateSearch(Enemy* owner) override;
	};





	// 徘徊(その場で周囲を見渡す)
	class Wander : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:Wander"; }
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
		const char* GetName() override { return "Ranged:Notice"; }
	};





	// 接近
	class Approach : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:Approach"; }
		bool IsChasing() override { return true; }
	};





	// 射撃ステート開始
	class ShotStart : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:ShotStart"; }
		bool IsChasing() override { return true; }
	};

	// 射撃溜め
	class ShotCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:ShotCharge"; }
		bool CanChangeState() override { return false; }
	};

	// 射撃実行
	class ShotExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:ShotExecute"; }
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
		const char* GetName() override { return "Ranged:ShotRecovery"; }
	};

	// 射撃間隔
	class ShotInterval : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:ShotInterval"; }
	};





	// ターゲットを見失ったとき
	class LostTarget : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:LostTarget"; }
		void UpdateSearch(Enemy* owner) override;

	private:
		enum class Phase
		{
			LOOK_AROUND,// 周囲を見渡す
			RETURN_HOME// 帰還
		};
		Phase _ePhase;

		VECTOR _vLookDir;// 見渡しの目標方向
		float _fLookTimer;// 現在の向きを維持するタイマー
		float _fLookDuration;// 現在の向きの維持時間
		int _lookCnt;// 見渡し回数
	};
}
