#include "PlayerBase.h"
#include "CameraManager.h"
#include "CameraDollyAddon.h"

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
		_playerState.StateReset();									// 状態リセット
		_playerState.combatState   = PLAYER_COMBAT_STATE::DEATH;	// 死亡ステートにする

		// アニメーション切り替え
		ProcessPlayAnimation();

		// カメラ演出テスト
		if (!_bDeathCameraSet)
		{
			_cameraManager->SetCameraType(CAMERA_TYPE::EVENT_CAMERA);
			auto zoom = std::make_shared<CameraDollyAddon>();
			zoom->StartDolly(-200.0f, 9999.0f, 240.0f);
			_cameraManager->AddAddon(zoom);
			_bDeathCameraSet = true;
		}
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
			static int timer = 0;
			timer++;
			if (timer > 120)
			{
				_bIsDeathAnimComplete = true;// 死亡アニメーション完了フラグを立てる
				timer = 0;
			}
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

