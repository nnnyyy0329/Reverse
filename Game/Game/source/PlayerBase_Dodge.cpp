// 担当 : 成田

#include "PlayerBase.h"
#include "StaminaManager.h"

namespace
{
	const float DODGE_STAMINA_COST = 10.0f;	// 回避時のスタミナ消費量
}

// 回避情報初期化
void PlayerBase::InitializeDodgeData()
{
	if(_dodgeSystem == nullptr){ return; }

	// 子クラスから回避情報の取得
	DodgeConfig playerDodgeConfig = GetDodgeConfig();

	// キャラタイプに応じた回避キャラタイプを決定
	DODGE_CHARA dodgeCharaType = DODGE_CHARA::NONE;

	// キャラタイプに応じた回避キャラタイプを決定
	switch(GetCharaType())
	{
		case CHARA_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::SURFACE_PLAYER;
			break;
		}

		case CHARA_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::INTERIOR_PLAYER;
			break;
		}

		case CHARA_TYPE::BULLET_PLAYER: // 弾プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::BULLET_PLAYER;
			break;
		}

		default:
			return;
	}

	// プレイヤーの回避設定を登録
	_dodgeSystem->RegisterCharaConfig(dodgeCharaType, playerDodgeConfig);
}

// 回避呼び出し
void PlayerBase::CallProcessDodge()
{
	if(_dodgeSystem == nullptr) { return; }	// 回避システムがなければ処理しない

	// 回避処理
	ProcessDodge();

	// 回避終了チェック
	ProcessEndDodge();
}

// 回避処理
void PlayerBase::ProcessDodge()
{
	// 回避入力があったら回避を呼び出す
	if((_playerState.movementState == PLAYER_MOVEMENT_STATE::WAIT ||
		_playerState.movementState == PLAYER_MOVEMENT_STATE::WALK ||
		_playerState.movementState == PLAYER_MOVEMENT_STATE::RUN)
		&& StaminaManager::GetInstance()->CanDodge()
		&& _trg & PAD_INPUT_3)
	{
		// 回避開始
		ProcessStartDodge();

		// 回避開始時にスタミナを消費
		StaminaManager::GetInstance()->ConsumeStamina(DODGE_STAMINA_COST);
	}
}

// 回避開始処理
void PlayerBase::ProcessStartDodge()
{
	// 回避可能かチェック
	if(!CanDodge()){ return; }	// 回避不可能なら処理しない

	// キャラタイプに応じた回避キャラタイプを決定
	DODGE_CHARA dodgeCharaType = DODGE_CHARA::NONE;

	// キャラタイプに応じた回避キャラタイプを決定
	switch(GetCharaType())
	{
		case CHARA_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::SURFACE_PLAYER;
			break;
		}

		case CHARA_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::INTERIOR_PLAYER;
			break;
		}

		case CHARA_TYPE::BULLET_PLAYER: // 弾プレイヤー
		{
			dodgeCharaType = DODGE_CHARA::BULLET_PLAYER;
			break;
		}

		default:
			return; 
	}

	// 回避呼び出し
	_dodgeSystem->CallDodge(shared_from_this(), dodgeCharaType);
	_playerState.combatState = PLAYER_COMBAT_STATE::DODGE;
}

// 回避終了処理
void PlayerBase::ProcessEndDodge()
{
	if(_playerState.combatState != PLAYER_COMBAT_STATE::DODGE){ return; }	// 回避中でなければ処理しない

	// 回避が終了した場合
	if(!_dodgeSystem->IsDodging() && _dodgeSystem->GetDodgeState() == DODGE_STATE::INACTIVE)
	{
		// 古いステータスを保持
		PLAYER_COMBAT_STATE oldStatus = _playerState.combatState;

		// ステータス変更後、アニメーション切り替え
		_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT; // 通常状態に戻る

		_oldPlayerState.combatState = oldStatus;	// 古いステータスを攻撃状態に設定
		ProcessPlayAnimation();						// アニメーション切り替え実行
		_oldPlayerState = _playerState;				// 切り替え後に更新
	}
}

// 回避可能かチェック
bool PlayerBase::CanDodge()
{
	if(_dodgeSystem != nullptr									||	// 回避システムが有効で
		_dodgeSystem->CanDodge()								||	// 回避中でなく
		!IsAttacking()											||	// 攻撃中でなく
		_playerState.combatState != PLAYER_COMBAT_STATE::HIT	||	// 被弾中でなく
		_playerState.combatState != PLAYER_COMBAT_STATE::DEATH)		// 死亡中じゃないなら
	{
		return true;
	}

	// 回避不可能
	return false;
}

// 回避中かチェック
bool PlayerBase::IsDodging()
{
	if(_dodgeSystem == nullptr) { return false; }

	// 回避中かチェック
	if(_playerState.combatState == PLAYER_COMBAT_STATE::DODGE ||
		_dodgeSystem->IsDodging())
	{
		return true;
	}

	return false;
}

// 無敵状態かチェック
bool PlayerBase::IsInvincible()
{
	if(_dodgeSystem == nullptr) { return false; }

	// 無敵状態かチェック
	return _dodgeSystem->GetIsInvincible();
}