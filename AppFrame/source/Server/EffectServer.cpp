#include "EffectServer.h"

// シングルトンインスタンスの実体
static EffectServer* _instance = nullptr;

EffectServer* EffectServer::GetInstance()
{
	if (_instance == nullptr) {// インスタンスがなければ生成
		_instance = new EffectServer();
	}
	return _instance;// インスタンスを返す
}

void EffectServer::Initialize()
{
	_resources.clear();// リソースマップをクリア
}

void EffectServer::Terminate()
{
	//  読み込んだ全エフェクトリソースを開放
	for (auto& pair : _resources) {
		DeleteEffekseerEffect(pair.second);
	}
	_resources.clear();// リソースマップをクリア

	// インスタンス削除
	if (_instance) {
		delete _instance;
		_instance = nullptr;
	}
}

void EffectServer::Update()
{
	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
}

void EffectServer::Render()
{
	// DXライブラリのカメラとEffekseerのカメラを同期する。
	Effekseer_Sync3DSetting();

	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer3D();
}

bool EffectServer::Load(const std::string& name, const char* path, float fScale)
{
	// すでに同じ名前で登録されていれば何もしない
	if (_resources.find(name) != _resources.end()) {
		return true;
	}

	// エフェクトリソースの読み込み
	int handle = LoadEffekseerEffect(path, fScale);

	if (handle == -1) return false;// 読み込み失敗

	_resources[name] = handle;// リソースマップに登録

	return true;
}

int EffectServer::Play(const std::string& name, VECTOR vPos)
{
	// 名前からリソースハンドルを検索
	auto it = _resources.find(name);
	if (it == _resources.end()) return -1;// 登録されていない

	int resourceHandle = it->second;// リソースハンドル取得

	// エフェクトの再生
	int playingHandle = PlayEffekseer3DEffect(resourceHandle);

	// 初期位置の設定
	SetPosPlayingEffekseer3DEffect(playingHandle, vPos.x, vPos.y, vPos.z);

	return playingHandle;// 再生中エフェクトハンドルを返す
}

void EffectServer::Stop(int playingHandle)
{
	// 再生中エフェクトの停止
	StopEffekseer3DEffect(playingHandle);
}

void EffectServer::SetPos(int playingHandle, VECTOR vPos)
{
	// 再生中エフェクトの位置更新
	SetPosPlayingEffekseer3DEffect(playingHandle, vPos.x, vPos.y, vPos.z);
}

void EffectServer::SetRot(int playingHandle, VECTOR vRot)
{
	// 回転の更新
	SetRotationPlayingEffekseer3DEffect(playingHandle, vRot.x, vRot.y, vRot.z);
}