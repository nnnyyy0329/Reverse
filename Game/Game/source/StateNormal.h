#pragma once
#include "EnemyState.h"

namespace Normal
{
	// 扇形視界判定
	bool IsTargetVisibleFan(Enemy* owner);

	// 待機
	class Idle : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* onwer) override;
		const char* GetName() override { return "Normal:Idle"; }
		void UpdateSearch(Enemy* owner) override;
	};

	// 徘徊
	class Wander : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Wander"; }
		void UpdateSearch(Enemy* owner) override;
	};

	// 発見
	class Notice : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Notice"; }
	};

	// 接近
	class Approach : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Approach"; }
		bool IsChasing() override { return true; }
	};

	// 攻撃ステート開始
	class AttackStart : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackStart"; }
		bool IsChasing() override { return true; }
	};

	// 攻撃溜め
	class AttackCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackCharge"; }
	};

	// 攻撃実行
	class AttackExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackExecute"; }

	private:
		bool _bHasCollision;// 攻撃コリジョンを出したか
	};

	// 攻撃後隙
	class AttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackRecovery"; }
	};

	// ターゲットを見失ったとき
	class LostTarget : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:LostTarget"; }
		void UpdateSearch(Enemy* owner) override;

	private:
		enum class Phase
		{
			LOOK_AROUND,// 周囲を見渡す
			TO_BASE,// 基準方向へ戻る
			WAIT,// 帰還前に待機
			RETURN_HOME// 帰還
		};
		Phase _ePhase;

		VECTOR _vLookDir;// 見渡しの目標方向
		VECTOR _vBaseDir;// 基準方向
		float _fBaseAngle;// 基準角度
		int _lookCnt;// 見渡し回数
		float _fWaitTimer;// 待機タイマー

		void SetNextLookDir();// 次の見渡し方向を設定
	};
}

