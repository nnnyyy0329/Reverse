#include "PlayerShieldSystem.h"
#include "PlayerBase.h"

PlayerShieldSystem::PlayerShieldSystem()
{
    _playerBase = nullptr;
}

PlayerShieldSystem::~PlayerShieldSystem()
{

}

bool PlayerShieldSystem::Initialize()
{
    // シールド初期化
    InitializeShieldData();

    return true;
}

bool PlayerShieldSystem::Terminate()
{
    return true;
}

bool PlayerShieldSystem::Process()
{
    // シールド処理呼び出し
    CallShield();

    return true;
}

bool PlayerShieldSystem::Render()
{
    return true;
}

// シールドデータ初期化
void PlayerShieldSystem::InitializeShieldData()
{
    // 子クラスからシールド設定を取得
    ShieldConfig config = GetShieldConfig();
}

// シールド処理呼び出し
void PlayerShieldSystem::CallShield()
{
    // シールド入力処理
    ProcessShieldInput();

    // シールド本体の処理
    if(_shieldBase){ _shieldBase->Process(); }
}

// シールド入力処理
void PlayerShieldSystem::ProcessShieldInput()
{
    if(!_shieldBase) return;

    // 入力取得
    int key = GetPlayerInput();

    // シールドに入力を渡す
    _shieldBase->UpdateGuardInput(key);
}

// プレイヤー入力取得
int PlayerShieldSystem::GetPlayerInput() const
{
    if(_playerBase)
    {
        // プレイヤーの入力受け取り
        return _playerBase->GetInput();
    }

    return 0;
}

// デフォルトのシールド設定を返す
ShieldConfig PlayerShieldSystem::GetShieldConfig()
{
    ShieldConfig config = {};

    config.charaType = SHIELD_CHARA::NONE;
    config.dotThreshold = 0.0f;
    config.blockAngleRange = 0.0f;
    config.blockDistance = 0.0f;
    config.guardStartupTime = 0.0f;
    config.guardEndTime = 0.0f;
    config.recoveryTime = 0.0f;
    config.staminaCost = 0.0f;
    config.blockEffectName = "";
    config.effectOffset = 0.0f;

    return config;
}