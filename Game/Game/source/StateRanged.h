#pragma once
#include "EnemyState.h"

namespace Ranged
{
	/*
	* Idle:待機
	* ↓プレイヤー索敵(TryDetectPlayer)
	* Detect:発見
	* ↓時間経過
	* Attack:攻撃
	* プレイヤーの方向を常に向く
	* 一定間隔で弾を発射し続ける
	* プレイヤーが範囲外にでたらIdleへ戻る
	*/

	// 待機
	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Idle"; }// 名前を返す(デバッグ用)
	};

	// 発見
	class Detect : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Detect"; }
	};

	// 攻撃
	class Attack : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Attack"; }
		bool IsChasing() const override { return true; }// 攻撃状態を追跡状態とみなす

	private:
		float _shotTimer;// 発射間隔タイマー
		// 弾の発射処理などをここに追加
		void Shoot(Enemy* owner);// 発射
	};
}
