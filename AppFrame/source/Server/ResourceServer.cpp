#include "ResourceServer.h"
#include "EffectServer.h"
#include "SoundServer.h"

// シングルトンインスタンスの実体
static ResourceServer* _instance = nullptr;

ResourceServer* ResourceServer::GetInstance()
{
	if (_instance == nullptr) {// インスタンスがなければ生成
		_instance = new ResourceServer();
	}
	return _instance;// インスタンスを返す
}

ResourceServer::~ResourceServer()
{
	Terminate();
}

void ResourceServer::Register(const std::string& name, const char* path, RESOURCE_TYPE type, float fScale)
{
	ResourceInfo info;
	info.name = name;
	info.path = path;
	info.type = type;
	info.fScale = fScale;
	_loadList.push_back(info);// ロード待ちリストに追加
}

void ResourceServer::StartLoadAsync()
{
	_totalCnt = static_cast<int>(_loadList.size());
	_asyncTotalCnt = 0;

	// 1 同期ロード
	SetUseASyncLoadFlag(FALSE);
	for (auto& res : _loadList) {
		int handle = -1;

		switch (res.type) {
		case RESOURCE_TYPE::Effect:
			EffectServer::GetInstance()->Load(res.name, res.path.c_str(), res.fScale);
			handle = 0;
			break;
		case RESOURCE_TYPE::Sound:
			SoundServer::GetInstance()->Load(res.name, res.path.c_str());
			handle = 0;
			break;
		default:
			break;
		}

		if (handle != -1) {
			_handleMap[res.name] = handle;
			res.handle = handle;
		}
	}

	// 2 非同期ロード
	SetUseASyncLoadFlag(TRUE);
	for (auto& res : _loadList) {
		int handle = -1;

		switch (res.type) {
		case RESOURCE_TYPE::Graph:
			++_asyncTotalCnt;
			handle = LoadGraph(res.path.c_str());
			break;
		case RESOURCE_TYPE::Model:
			++_asyncTotalCnt;
			handle = MV1LoadModel(res.path.c_str());
			break;
		default:
			break;
		}

		if (handle != -1) {
			_handleMap[res.name] = handle;
			res.handle = handle;
		}
	}
	SetUseASyncLoadFlag(FALSE);
}

float ResourceServer::GetLoadProgress()
{
	if (_asyncTotalCnt <= 0) { return 1.0f; }

	int remain = GetASyncLoadNum();
	if (remain < 0) { remain = 0; }
	if (remain > _asyncTotalCnt) { remain = _asyncTotalCnt; }

	return static_cast<float>(_asyncTotalCnt - remain) / static_cast<float>(_asyncTotalCnt);
}

bool ResourceServer::IsLoadComplete()
{
	return GetASyncLoadNum() == 0;// 非同期読み込み中の処理の数が0なら完了
}

void ResourceServer::Terminate()
{
	_loadList.clear();
	_handleMap.clear();
}

int ResourceServer::GetHandle(const std::string& name)
{
	auto it = _handleMap.find(name);
	if (it != _handleMap.end()) {
		return it->second;
	}
	return -1;// 見つからなかった
}
