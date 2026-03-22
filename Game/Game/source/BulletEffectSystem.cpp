#include "BulletEffectSystem.h"

// シングルトンインスタンス初期化
BulletEffectSystem* BulletEffectSystem::_instance = nullptr;

// シングルトンアクセス
BulletEffectSystem* BulletEffectSystem::GetInstance()
{
	return _instance;
}

// シングルトンインスタンス生成
void BulletEffectSystem::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new BulletEffectSystem();	// インスタンス生成
		_instance->Initialize();				// 初期化
	}
}

// シングルトンインスタンス破棄
void BulletEffectSystem::DestroyInstance()
{
	if(_instance != nullptr)
	{
		delete _instance;		// インスタンス破棄
		_instance = nullptr;	// ポインタをnullptrに設定
	}
}

BulletEffectSystem::BulletEffectSystem()
{

}

BulletEffectSystem::~BulletEffectSystem()
{

}

bool BulletEffectSystem::Initialize()
{
	return true;
}

bool BulletEffectSystem::Terminate()
{
	//// アクティブなエフェクトをすべて停止
	//for(auto& effect : _activeEffects)
	//{
	//	// エフェクト停止
	//	EffectServer::GetInstance()->Stop(effect.first);
	//}

	//// アクティブエフェクトマップの初期化
	//_activeEffects.clear();

	return true;
}

bool BulletEffectSystem::Process()
{
	return true;
}

int BulletEffectSystem::PlayBulletEffect(const std::string& effectName, const VECTOR& pos)
{
	// エフェクト名がないなら無効な値
	if(effectName.empty()){ return -1; }

	// エフェクトサーバー取得
	auto effectServer = EffectServer::GetInstance();
	if(!effectServer){ return -1; }

	// エフェクトの再生
	int handle = effectServer->Play(effectName, pos);

	//if(handle > 0)
	//{
	//	// エフェクトを有効にする
	//	_activeEffects[handle] = true;
	//}

	return handle;
}

void BulletEffectSystem::UpdateBulletEffectPos(int effectHandle, const VECTOR& pos)
{
	// エフェクトハンドルがないならスキップ
	if(effectHandle <= 0){ return; }

	// エフェクトサーバー取得
	auto effectServer = EffectServer::GetInstance();

	if(effectServer)
	{
		// 位置更新処理
		effectServer->SetPos(effectHandle, pos);
	}
}

void BulletEffectSystem::StopBulletEffect(int effectHandle)
{
	// エフェクトハンドルがないならスキップ
	if(effectHandle <= 0){ return; }

	// エフェクトサーバー取得
	auto effectServer = EffectServer::GetInstance();

	if(effectServer)
	{
		// エフェクト再生停止
		effectServer->Stop(effectHandle);
	}
}

void BulletEffectSystem::PlayBulletSound(const std::string& soundName)
{
	// サウンド名がないならスキップ
	if(soundName.empty()) { return; }

	// サウンドサーバー取得
	auto soundServer = SoundServer::GetInstance();
	
	if(soundServer)
	{
		// サウンド再生
		soundServer->Play(soundName, DX_PLAYTYPE_BACK);
	}
}
