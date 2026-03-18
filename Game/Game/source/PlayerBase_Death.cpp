#include "PlayerBase.h"
#include "CameraManager.h"
#include "CameraDollyAddon.h"

void PlayerBase::CallDeath()
{
	// 死亡処理
	ProcessDeath();

	// 死亡状態の更新
	UpdateDeathState();

	// 死亡後の処理
	UpdateAfterDeath();
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

		// カメラ演出開始
		StartDeathCameraEffect();
	}
}

void PlayerBase::StartDeathCameraEffect()
{
	// カメラ演出テスト
	if(!_bIsDeathCameraSet)
	{
		// カメラタイプを演出用カメラに切り替える
		_cameraManager->SetCameraType(CAMERA_TYPE::EVENT_CAMERA);

		// カメラドリーアドオンを作成して追加する
		auto zoom = std::make_shared<CameraDollyAddon>();

		// ドリー移動を開始する（引きで、全体の持続時間9999秒、移行時間240フレーム）
		zoom->StartDolly(-200.0f, 9999.0f, 240.0f);

		// カメラマネージャーにアドオンを追加
		_cameraManager->AddAddon(zoom);

		// カメラ切り替えフラグを立てる
		_bIsDeathCameraSet = true;
	}
}

void PlayerBase::UpdateDeathState()
{
	//if(_playerState.combatState == PLAYER_COMBAT_STATE::DEATH)
	//{
	//	// アニメーションが完了したかチェック
	//	AnimManager* animManager = GetAnimManager();
	//	if(animManager->IsAnimationFinished())
	//	{
	//		static int timer = 0;
	//		timer++;
	//		if (timer > 60)
	//		{
	//			_bIsDeathAnimComplete = true;// 死亡アニメーション完了フラグを立てる
	//			timer = 0;
	//		}
	//	}

	//	// 死亡フラグを立てる
	//	_bIsDead = true;				
	//}

	if(_playerState.combatState != PLAYER_COMBAT_STATE::DEATH){ return; }
	
	// アニメーションマネージャーを取得
	AnimManager* animManager = GetAnimManager();	

	// アニメーションが終了したかチェック
	if(animManager->IsAnimationFinished())
	{
		// 死亡アニメーション完了フラグを立てる
		_bIsDeathAnimComplete = true;	

		// 死亡フラグを立てる
		_bIsDead = true;				
	}
}

void PlayerBase::UpdateAfterDeath()
{
	if(!IsDeath()){ return; }

	// 状態リセット
	_playerState.StateReset();
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

