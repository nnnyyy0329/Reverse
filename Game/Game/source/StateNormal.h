#pragma once
#include "EnemyState.h"

namespace Normal
{
	// îŒ`‹ŠE”»’è
	bool IsTargetVisibleFan(Enemy* owner);

	// ‘Ò‹@
	class Idle : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* onwer) override;
		const char* GetName() override { return "Normal:Idle"; }
		void UpdateSearch(Enemy* owner) override;
	};

	// œpœj
	class Wander : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Wander"; }
		void UpdateSearch(Enemy* owner) override;
	};

	// ”­Œ©
	class Notice : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Notice"; }
	};

	// Ú‹ß
	class Approach : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:Approach"; }
		bool IsChasing() override { return true; }
	};

	// UŒ‚—­‚ß
	class AttackCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackCharge"; }
	};

	// UŒ‚Às
	class AttackExecute : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackExecute"; }

	private:
		bool _bHasCollision;// UŒ‚ƒRƒŠƒWƒ‡ƒ“‚ğo‚µ‚½‚©
	};

	// UŒ‚ŒãŒ„
	class AttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Normal:AttackRecovery"; }
	};

	// ƒ^[ƒQƒbƒg‚ğŒ©¸‚Á‚½‚Æ‚«
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
			LOOK_AROUND,// üˆÍ‚ğŒ©“n‚·
			TO_BASE,// Šî€•ûŒü‚Ö–ß‚é
			WAIT,// ‹AŠÒ‘O‚É‘Ò‹@
			RETURN_HOME// ‹AŠÒ
		};
		Phase _ePhase;

		VECTOR _vLookDir;// Œ©“n‚µ‚Ì–Ú•W•ûŒü
		VECTOR _vBaseDir;// Šî€•ûŒü
		float _fBaseAngle;// Šî€Šp“x
		int _lookCnt;// Œ©“n‚µ‰ñ”
		float _fWaitTimer;// ‘Ò‹@ƒ^ƒCƒ}[

		void SetNextLookDir();// Ÿ‚ÌŒ©“n‚µ•ûŒü‚ğİ’è
	};
}

