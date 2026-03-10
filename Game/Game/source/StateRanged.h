#pragma once
#include "EnemyState.h"
#include "BulletManager.h"

namespace Ranged
{
	// 扇形視界判定
	bool IsTargetVisibleFan(Enemy* owner);

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



	// 後退
	class Retreat : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() override { return "Ranged:Retreat"; }
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
		BulletConfig GetBulletConfig(Enemy* owner);
		const char* GetName() override { return "Ranged:ShotExecute"; }
		STATE_PRIORITY GetPriority() override { return STATE_PRIORITY::HIGH; }

	private:
		bool _bHasShot;
		float _fShotTimer = 0.0f;// 実際に弾を発射する時間
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
			TO_BASE,// 基準方向へ戻る
			WAIT,// 帰還前に待機
			RETURN_HOME,// 帰還
			SET_DIR// 初期向きへ向きなおす
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
