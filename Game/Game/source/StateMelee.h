#pragma once
#include "EnemyState.h"

namespace Melee
{
	/*
	* Idle:待機
	* ↑↓時間経過
	* Move:自動移動
	* ↓プレイヤー索敵(TryDetectPlayer)
	* Detect:発見
	* ↓時間経過
	* Chase:追跡(距離が離れすぎたらIdleへ)
	* ↑↓プレイヤーが攻撃射程内に入ったら
	* Attack:攻撃(Chaseに戻り距離を再確認)
	*/

	// 円形視界判定(距離のみチェック)
	bool IsTargetVisible(Enemy* owner);

	// 待機
	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Idle"; }// 名前を返す(デバッグ用)
	};

	// 自動移動
	class Move : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Move"; }
	};

	// 発見:見つけた瞬間の硬直
	class Detect : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Detect"; }
	};

	// 追跡
	class Chase : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:Chase"; }
		bool IsChasing() const override { return true; }// 追跡状態である
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

	// 初期位置への復帰
	class ReturnHome : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Melee:ReturnHome"; }
	};
}

