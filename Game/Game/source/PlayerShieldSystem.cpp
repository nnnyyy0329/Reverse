#include "PlayerShieldSystem.h"

PlayerShieldSystem::PlayerShieldSystem()
{
	// シールドベースクラス作成
    _shieldBase = std::make_shared<ShieldBase>();
}

PlayerShieldSystem::~PlayerShieldSystem()
{

}

bool PlayerShieldSystem::Initialize()
{
    if(!_shieldBase){ return false; }

    return _shieldBase->Initialize();
}

bool PlayerShieldSystem::Terminate()
{
    if(!_shieldBase){ return false; }

	return _shieldBase->Terminate();
}

bool PlayerShieldSystem::Process()
{
    if(!_shieldBase){ return false; }

	return _shieldBase->Process();
}

bool PlayerShieldSystem::Render()
{
    if(!_shieldBase){ return false; }

	return _shieldBase->Render();
}

// シールド入力処理
void PlayerShieldSystem::ProcessInput(int key)
{
    if(!_shieldBase){ return; }

    // ガード入力更新
    //_shieldBase->UpdateGuardInput(key);
}

// シールドがアクティブかチェック
bool PlayerShieldSystem::IsActive() const
{
    if(!_shieldBase){ return false; }

	// シールドがアクティブかチェック
    return _shieldBase->IsActive();
}

// 所有者設定
void PlayerShieldSystem::SetOwner(std::shared_ptr<CharaBase> owner)
{
    if(!_shieldBase){ return; }

    _shieldBase->SetOwner(owner);
}

// シールド設定登録
void PlayerShieldSystem::SetShieldConfig(const ShieldConfig& config)
{
    if(!_shieldBase){ return; }

    _shieldBase->SetShieldConfig(config);
}