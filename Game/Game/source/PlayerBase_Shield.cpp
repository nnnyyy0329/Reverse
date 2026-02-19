#include "PlayerBase.h"

// シールド関係Process呼び出し用関数
void PlayerBase::CallProcessShield()
{
    if(!_shieldSystem){ return; }

    // シールドシステムの更新
    _shieldSystem->Process();

    // シールド入力処理
    ProcessShieldInput();

    // シールド処理
    ProcessShield();
}

// シールド入力処理
void PlayerBase::ProcessShieldInput()
{
    if(!_shieldSystem) { return; }

    // 攻撃中や回避中、被弾中はシールド入力を受け付けない
    if(IsAttacking() || IsDodging() || IsHitStop()) { return; }

    // シールドシステムに入力を渡す
    _shieldSystem->ProcessInput(_key);
}

// シールド処理
void PlayerBase::ProcessShield()
{
    if(!_shieldSystem) { return; }

    // シールド状態による移動制限
    if(IsShielding())
    {
        // シールド中のアニメーション処理
        if(_playerState.combatState != PLAYER_COMBAT_STATE::GUARD)
        {
            _oldPlayerState = _playerState;
            _playerState.combatState = PLAYER_COMBAT_STATE::GUARD;
            ProcessPlayAnimation();
        }
    }
    else
    {
        // シールドが非アクティブになったらガード状態を解除
        if(_playerState.combatState == PLAYER_COMBAT_STATE::GUARD)
        {
            _oldPlayerState = _playerState;
            _playerState.combatState = PLAYER_COMBAT_STATE::NONE;
            ProcessPlayAnimation();
        }
    }
}

// シールド中かチェック
bool PlayerBase::IsShielding()
{
    if(!_shieldSystem){ return false; }

	// シールドがアクティブかチェック
    return _shieldSystem->IsActive();
}