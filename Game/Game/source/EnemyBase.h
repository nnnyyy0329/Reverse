#pragma once
#include "CharaBase.h"

class EnemyBase : public CharaBase
{
public:
	EnemyBase();
	virtual ~EnemyBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ描画

	float GetVisionRange() const { return _fVisionRange; }// 索敵距離取得
	float GetVisionCosAngle() const { return _fVisionCosAngle; }// 判定で使うcos値

	void OnDetectPlayer(std::shared_ptr<CharaBase> target);// プレイヤー発見時の処理

protected:
	enum class ENEMY_STATE {// 敵AIの状態
		NONE,
		IDLE,// 待機
		MOVE,// 自動移動
		DETECT,// 発見
		CHASE,// 追跡
		ATTACK,// 攻撃
		_EOT_,
	};
	ENEMY_STATE _eCurrentState;// 現在の敵の状態
	ENEMY_STATE _eOldState;// 前フレームの敵の状態

	float _fStateTimer;// 状態の経過時間

	VECTOR _vHomePos;// 敵の初期位置

	void ChangeState(ENEMY_STATE newState);// 敵の状態変更

	void EnemyIdleProcess();// 待機状態の処理
	void EnemyMoveProcess();// 自動移動状態の処理
	void EnemyDetectProcess();// 発見状態の処理
	void EnemyChaseProcess();// 追跡状態の処理
	void EnemyAttackProcess();// 攻撃状態の処理

	void SetVisionAngle(float angle);// 視界の角度設定

	float _fVisionRange;// 索敵距離
	float _fVisionAngle;// 視界の角度(半分)
	float _fVisionCosAngle;// 視界の角度のcos値

	std::shared_ptr<CharaBase> _targetPlayer;// 追跡用
};

