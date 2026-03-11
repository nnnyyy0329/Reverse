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
		const char* GetName() override { return "Tank:Idle"; }// 名前を返す(デバッグ用)
	};



	// 接近
	class Approach : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:Approach"; }
		bool IsChasing() override { return true; }// 接近状態である
	};



	// 攻撃判定ステート
	class Attack : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:Attack"; }

	private:
		bool _bDoCharge = false;// 突進攻撃を行うか
		float _fJudgeTimer = 0.0f;
	};



	// 一段目攻撃前の予備動作
	class FirstAttackPrepare : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:FirstAttackPrepare"; }
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
		const char* GetName() override { return "Tank:FirstAttackExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bAttackStarted = false;
	};

	// 一段目攻撃の後隙
	class FirstAttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:FirstAttackRecovery"; }
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
		const char* GetName() override { return "Tank:SecondAttackPrepare"; }
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
		const char* GetName() override { return "Tank:SecondAttackExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }	

	private:
		bool _bAttackStarted = false;
	};

	// 二段目攻撃の後隙
	class SecondAttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:SecondAttackRecovery"; }
		bool CanChangeState() override { return _bIsCompleted; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bIsCompleted = false;// 後隙完了フラグ
		bool _bJudged = false;// 三段目にいくかの判定をしたか
	};



	// 三段目攻撃前の予備動作
	class ThirdAttackPrepare : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:ThirdAttackPrepare"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }
	};

	// 三段目攻撃の実行
	class ThirdAttackExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() override { return "Tank:ThirdAttackExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bAttackStarted = false;
	};

	// 三段目攻撃の後隙
	class ThirdAttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:ThirdAttackRecovery"; }
		bool CanChangeState() override { return _bIsCompleted; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bIsCompleted = false;// 後隙完了フラグ
	};



	// 突進前の予備動作
	class ChargePrepare : public EnemyState
	{
	public:
		ChargePrepare(VECTOR vChargeTarget) : _vChargeTarget(vChargeTarget) {}

		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:ChargePrepare"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		VECTOR _vChargeTarget = VGet(0.0f, 0.0f, 0.0f);// 突進先(遷移時に固定)
	};

	// 突進の実行
	class ChargeExecute : public EnemyState
	{
	public:
		ChargeExecute(VECTOR vChargeDir, VECTOR vChargeTarget)
			: _vChargeDir(vChargeDir), _vChargeTarget(vChargeTarget) {}

		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() override { return "Tank:ChargeExecute"; }
		bool CanChangeState() override { return false; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		VECTOR _vChargeDir = VGet(0.0f, 0.0f, 0.0f);// 突進方向
		VECTOR _vChargeTarget = VGet(0.0f, 0.0f, 0.0f);// 到達座標
	};

	// 突進の後隙
	class ChargeRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Tank:ChargeRecovery"; }
		bool CanChangeState() override { return _bIsCompleted; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bIsCompleted = false;
	};
}