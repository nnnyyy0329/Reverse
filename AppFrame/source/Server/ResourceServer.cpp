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
	_totalCnt = static_cast<int>(_loadList.size());// 総ロード数を保存

	// DXライブラリの非同期ロードをONにする
	SetUseASyncLoadFlag(TRUE);

	// リストの先頭から順にロードを開始する
	for (auto& res : _loadList) {
		int handle = -1;

		switch (res.type) {
		case RESOURCE_TYPE::Graph:// 画像
				handle = LoadGraph(res.path.c_str());
				break;
		case RESOURCE_TYPE::Model:// 3Dモデル
			handle = MV1LoadModel(res.path.c_str());
			break;
		case RESOURCE_TYPE::Effect:// エフェクト
			SetUseASyncLoadFlag(FALSE);
			// EffectServer経由でロード
			EffectServer::GetInstance()->Load(res.name, res.path.c_str(), res.fScale);
			handle = 0;// ハンドルはEffectServer側で管理する
			SetUseASyncLoadFlag(TRUE);
			break;
		case RESOURCE_TYPE::Sound:// サウンド
			// SoundServer経由で読み込む（SoundServerが内部でハンドル管理）
			SoundServer::GetInstance()->Load(res.name, res.path.c_str());
			handle = 0; // SoundServer側で管理するためここでは 0 をセット
			break;
		}

		// マップに登録
		if (handle != -1) {
			_handleMap[res.name] = handle;
			res.handle = handle;// 念のためにリストにも保存
		}
	}

	// 非同期ロードをOFFに戻す
	SetUseASyncLoadFlag(FALSE);
}

float ResourceServer::GetLoadProgress()
{
	int remain = GetASyncLoadNum();// 非同期読み込み中の処理の数を取得

	if (_totalCnt == 0) return 1.0f;// ロードするものがない場合は完了

	// 総数 - 残り / 総数 = 進捗率
	return static_cast<float>(_totalCnt - remain) / static_cast<float>(_totalCnt);
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
