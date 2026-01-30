#pragma once
#pragma once
#include <map>
#include <string>
#include <mutex>
#include "DxLib.h"

class SoundServer
{
public:
	static SoundServer* GetInstance();

	void Initialize();
	void Terminate();

	bool Load(const std::string& name, const char* path);
	bool Unload(const std::string& name);
	bool IsLoaded(const std::string& name) const;

	// 戻り値は DxLib のサウンドハンドル（>=0）または -1
	int Play(const std::string& name, int playType = DX_PLAYTYPE_BACK);
	void Stop(const std::string& name);
	void StopByHandle(int soundHandle);

	void SetVolume(const std::string& name, int volume); // 0-255
	void SetVolumeByHandle(int soundHandle, int volume);

	void StopAll();
	void UnloadAll();

private:
	SoundServer() {}
	~SoundServer() {}

	SoundServer(const SoundServer&) = delete;
	SoundServer& operator=(const SoundServer&) = delete;

	mutable std::mutex _mtx;
	std::map<std::string, int> _resources;
};