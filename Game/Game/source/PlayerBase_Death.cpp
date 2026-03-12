#include "PlayerBase.h"
#include "CameraManager.h"
#include "CameraDollyAddon.h"

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

		// カメラ演出テスト
		/*_cameraManager->Reset();
		_cameraManager->SetCameraType(CAMERA_TYPE::EVENT_CAMERA);
		auto zoom = std::make_shared<CameraDollyAddon>();
		zoom->StartDolly(-250.0f, 300.0f, 60.0f);
		_cameraManager->AddAddon(zoom);*/
	}
}

void PlayerBase::UpdateDeathState()
{
	if(_playerState.combatState == PLAYER_COMBAT_STATE::DEATH)
	{
		// アニメーションが完了したかチェック
		AnimManager* animManager = GetAnimManager();
		if(animManager->IsAnimationFinished())
		{
			static int timer = 0;
			timer++;
			if (timer > 60)
			{
				_bIsDeathAnimComplete = true;// 死亡アニメーション完了フラグを立てる
				timer = 0;
			}
		}

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

