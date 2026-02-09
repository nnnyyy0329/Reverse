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





	// 攻撃ステート開始(ここでどの攻撃をするか決定)
	class AttackStart : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:AttackStart"; }
		bool IsChasing() const override { return true; }

	private:
		enum class AttackType
		{
			NORMAL,
			RUSH
		};
		AttackType _eAttackType;
	};

	// 攻撃溜め(その場で向きを追従)
	class AttackCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:AttackCharge"; }
		bool CanChangeState() override { return false; }
	};

	// 攻撃実行(ここで攻撃判定を出す)
	class AttackExecute : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "Melee:AttackExecute"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bHasCollision;// 攻撃コリジョンを出したか
	};

	// 攻撃後隙
	class AttackRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:AttackRecovery"; }
	};

	// 突進攻撃溜め
	class RushCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "RushCharge"; }
		bool CanChangeState() override { return false; }

	private:
		VECTOR _vRushDir;
	};

	// 突進攻撃実行
	class RushExecute : public EnemyState
	{
	public:
		RushExecute(VECTOR vRushDirection) : _vRushDir(vRushDirection), _bHasCollision(false), _fCurrentSpeed(0.0f) {}
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() const override { return "RushExecute"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		VECTOR _vRushDir;// 突進方向
		VECTOR _vStartPos;// 突進開始位置
		VECTOR _vTargetPos;// 突進目標位置
		bool _bHasCollision;// 突進攻撃コリジョンを出したか
		bool _bReachedTarget;// 目標位置に到達したか
		float _fCurrentSpeed;// 現在の突進速度
		float _fReachTimer;// 目標到達後のタイマー
	};

	// 突進攻撃後隙
	class RushRecovery : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "RushRecovery"; }
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

