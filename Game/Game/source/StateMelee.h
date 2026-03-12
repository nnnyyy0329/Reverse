#pragma once
#include "EnemyState.h"
#include "StateNormal.h"

namespace Melee
{
	// 扇形視界判定
	bool IsTargetVisibleFan(Enemy* owner);

	// 待機
	using Idle = Normal::Idle;
	// 徘徊
	using Wander = Normal::Wander;
	// 発見
	using Notice = Normal::Notice;
	// 接近
	using Approach = Normal::Approach;
	// 攻撃溜め
	using AttackCharge = Normal::AttackCharge;
	// 攻撃実行
	using AttackExecute = Normal::AttackExecute;
	// 攻撃後隙
	using AttackRecovery = Normal::AttackRecovery;
	// ターゲットを見失ったとき
	using LostTarget = Normal::LostTarget;



	// 攻撃ステート開始(ここでどの攻撃をするか決定)
	class AttackStart : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Melee:AttackStart"; }
		bool IsChasing() override { return true; }

	private:
		enum class AttackType
		{
			NORMAL,
			RUSH
		};
		AttackType _eAttackType;
	};

	// 突進攻撃溜め
	class RushCharge : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "RushCharge"; }
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
		const char* GetName() override { return "RushExecute"; }
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
		const char* GetName() override { return "RushRecovery"; }
	};



	// 対峙
	class Confront : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Melee:Confront"; }

	private:
		float _fDuration;// 対峙時間
		int _direction;// 1:右回り, -1:左回り
	};



	// 反撃
	class CounterAttack : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		void Exit(Enemy* owner) override;
		const char* GetName() override { return "Melee:CounterAttack"; }
		//STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bHasCollision;
	};
}

