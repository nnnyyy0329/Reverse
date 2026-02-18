#include "PlayerBase.h"

// 死亡関係関数呼び出し
void PlayerBase::CallDeath()
{
	// 死亡処理
	ProcessDeath();

	// 死亡アニメーションが再生し終わったか
	CheckDeathAnimFinished();

	// 死亡したか
	CheckDeath();
}

// 死亡処理
void PlayerBase::ProcessDeath()
{
	// 体力が0以下なら死亡処理
	if(!IsAlive())
	{
		_playerState.movementState = PLAYER_MOVEMENT_STATE::NONE;	
		_playerState.attackState   = PLAYER_ATTACK_STATE::NONE;
		_playerState.shootState	   = PLAYER_SHOOT_STATE::NONE;
		_playerState.combatState   = PLAYER_COMBAT_STATE::DEATH;	// 死亡ステートにする

		// アニメーション切り替え
		ProcessPlayAnimation();
	}
}

// 死亡アニメーションが再生し終わったか
void PlayerBase::CheckDeathAnimFinished()
{
	if(_playerState.combatState == PLAYER_COMBAT_STATE::DEATH)
	{
		// アニメーションが完了したかチェック
		AnimManager* animManager = GetAnimManager();
		if(animManager->IsAnimationFinished())
		{
			_bIsDeathAnimComplete = true; // 死亡アニメーション完了フラグを立てる
		}
	}
}

// 死亡したか
void PlayerBase::CheckDeath()
{
	if(_bIsDeathAnimComplete)
	{
		_bIsDead = true;
	}
}

// 死亡したか
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

// プレイヤーが生きているから
bool PlayerBase::IsAlive()const
{
	// 体力が0より大きいなら
	if(_fLife > 0.0f)
	{
		// 0以上
		return true;
	}

	return false;
}

// 死亡ステートになったか
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

