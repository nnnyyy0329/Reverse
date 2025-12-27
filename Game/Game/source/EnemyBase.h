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

protected:
	enum class ENEMY_STATE {// 敵AIの状態
		NONE,
		IDLE,// 待機
		MOVE,// 自動移動
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
	void EnemyChaseProcess();// 追跡状態の処理
	void EnemyAttackProcess();// 攻撃状態の処理

};

