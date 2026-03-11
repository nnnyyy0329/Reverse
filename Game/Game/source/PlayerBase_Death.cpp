#include "PlayerBase.h"

void PlayerBase::CallDeath()
{
	// 死亡処理
	ProcessDeath();

	// 死亡状態の更新
	UpdateDeathState();
}

void PlayerBase::ProcessDeath()
{
	// 体力が0以下なら死亡処理
	if(!IsAlive())
	{
		// 状態リセット
		_playerState.StateReset();								

		// 死亡ステートにする
		_playerState.combatState = PLAYER_COMBAT_STATE::DEATH;	

		// アニメーション切り替え
		ProcessPlayAnimation();
	}
}

void PlayerBase::UpdateDeathState()
{
	if(_playerState.combatState != PLAYER_COMBAT_STATE::DEATH){ return; }
	
	// アニメーションマネージャー取得
	AnimManager* animManager = GetAnimManager();	

	// アニメーションが完了したかチェック
	if(animManager->IsAnimationFinished())
	{
		// 死亡アニメーション完了フラグを立てる
		_bIsDeathAnimComplete = true;	

		// 死亡フラグを立てる
		_bIsDead = true;				
	}
}

bool PlayerBase::IsDeath()const
{
	// 死亡フラグが有効か
	if(_bIsDead)
	{
		// 死亡
		return true;
	}

	return false;
}

bool PlayerBase::IsAlive()const
{
	// 体力が0より大きいなら
	if(_fLife > 0.0f)
	{
		// 0以上なら生きている
		return true;
	}

	return false;
}

bool PlayerBase::IsStateDeath()const
{
	// 死亡ステートになったか
	if(_playerState.combatState == PLAYER_COMBAT_STATE::DEATH)
	{
		// 死亡ステートになった
		return true;
	}

	return false;
}

