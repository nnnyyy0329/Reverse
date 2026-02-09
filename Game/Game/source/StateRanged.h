#pragma once
#include "EnemyState.h"

namespace Ranged
{
	// 扇形視界判定(距離と角度をチェック)
	bool IsTargetVisible(Enemy* owner);

	// 待機
	class Idle : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Idle"; }// 名前を返す(デバッグ用)
		void UpdateSearch(Enemy* owner) override;

	private:
		float _fLookAroundTimer;// 見渡すタイマー
		float _fTargetAngle;// 目標とする角度(ラジアン)
		bool _bLookingLeft;// 左を向いているか(true:左, false:右)
		bool _bisRotating;// 回転中かどうか
		VECTOR _vInitialDir;//見渡し開始時の向き
	};





	// 発見
	class Notice : public EnemyState
	{
		public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Notice"; }
	};





	// 攻撃
	class Attack : public EnemyState
	{
	public:
		void Enter(Enemy* owner) override;
		std::shared_ptr<EnemyState> Update(Enemy* owner) override;
		const char* GetName() const override { return "Ranged:Attack"; }
		bool IsChasing() const override { return true; }// 攻撃状態を接近状態とみなす

	private:
		float _shotTimer;// 発射間隔タイマー
		// 弾の発射処理などをここに追加
		void Shoot(Enemy* owner);// 発射
	};
}
