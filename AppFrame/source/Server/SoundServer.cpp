#include "SoundServer.h"
#include <utility>

// シングルトンインスタンスの実体
static SoundServer* _instance = nullptr;

SoundServer* SoundServer::GetInstance()
{
	if(_instance == nullptr) {
		_instance = new SoundServer();
	}
	return _instance;
}

void SoundServer::Initialize()
{
	std::lock_guard<std::mutex> lk(_mtx);
	_resources.clear();
	_masterVolume = 255; // 初期マスター音量
}

void SoundServer::Terminate()
{
	// 全サウンド停止・解放
	UnloadAll();

	// インスタンス削除
	if(_instance) {
		delete _instance;
		_instance = nullptr;
	}
}

bool SoundServer::Load(const std::string& name, const char* path)
{
	std::lock_guard<std::mutex> lk(_mtx);
	if(_resources.find(name) != _resources.end()) {
		return true; // 既に読み込み済み
	}

	int handle = LoadSoundMem(path);
	if(handle < 0) {
		return false; // 読み込み失敗
	}

	ChangeVolumeSoundMem(_masterVolume, handle);

	_resources[name] = handle;
	return true;
}

bool SoundServer::Unload(const std::string& name)
{
	std::lock_guard<std::mutex> lk(_mtx);
	auto it = _resources.find(name);
	if(it == _resources.end()) return false;

	int h = it->second;
	StopSoundMem(h);
	DeleteSoundMem(h);
	_resources.erase(it);
	return true;
}

bool SoundServer::IsLoaded(const std::string& name) const
{
	std::lock_guard<std::mutex> lk(_mtx);
	return _resources.find(name) != _resources.end();
}

int SoundServer::Play(const std::string& name, int playType)
{
	std::lock_guard<std::mutex> lk(_mtx);
	auto it = _resources.find(name);
	if(it == _resources.end()) return -1;

	int h = it->second;
	// PlaySoundMem の戻り値は成功/失敗だが、呼び出し側で停止したいのでハンドルを返す
	PlaySoundMem(h, playType);

	// 再生時にマスター音量を適用
	ChangeVolumeSoundMem(_masterVolume, h);
	return h;
}

void SoundServer::Stop(const std::string& name)
{
	std::lock_guard<std::mutex> lk(_mtx);
	auto it = _resources.find(name);
	if(it == _resources.end()) return;
	StopSoundMem(it->second);
}

void SoundServer::StopByHandle(int soundHandle)
{
	if(soundHandle < 0) return;
	StopSoundMem(soundHandle);
}

void SoundServer::SetVolume(const std::string& name, int volume)
{
	std::lock_guard<std::mutex> lk(_mtx);
	auto it = _resources.find(name);
	if(it == _resources.end()) return;
	ChangeVolumeSoundMem(volume, it->second);
}

void SoundServer::SetVolumeByHandle(int soundHandle, int volume)
{
	if(soundHandle < 0) return;
	ChangeVolumeSoundMem(volume, soundHandle);
}

//void SoundServer::SetMasterVolume(int volume)
//{
//	std::lock_guard<std::mutex> lk(_mtx);
//	_masterVolume = std::clamp(volume, 0, 255);
//	// 現在読み込まれている全ハンドルに適用
//	for(auto& p : _resources) {
//		ChangeVolumeSoundMem(_masterVolume, p.second);
//	}
//}

void SoundServer::StopAll()
{
	std::lock_guard<std::mutex> lk(_mtx);
	for(auto& p : _resources) {
		StopSoundMem(p.second);
	}
}

void SoundServer::UnloadAll()
{
	std::lock_guard<std::mutex> lk(_mtx);
	for(auto& p : _resources) {
		StopSoundMem(p.second);
		DeleteSoundMem(p.second);
	}
	_resources.clear();
}