#pragma once
#include <string>
#include <vector>
#include <map>
#include "DxLib.h"

// リソースの種類
enum class RESOURCE_TYPE
{
	Graph,
	Model,
	Effect,
	Sound,
};

// リソースごとの情報
struct ResourceInfo
{
	std::string name;// 呼び出し名
	std::string path;// ファイルパス
	RESOURCE_TYPE type;// 種類
	int handle;// ロード後のハンドル

	// 追加可能
	float fScale;// スケール(エフェクト用)
};

class ResourceServer
{
public:
	// シングルトンインスタンス取得
	static ResourceServer* GetInstance();

	// 登録:リストに登録するだけ
	void Register(const std::string& name, const char* path, RESOURCE_TYPE type, float fScale);

	// 非同期ロード開始
	void StartLoadAsync();

	// ロード進捗率(0.0f ~ 1.0f)
	float GetLoadProgress();

	// ロード完了したか
	bool IsLoadComplete();

	void Terminate();// 終了

	int GetHandle(const std::string& name);// ハンドル取得

private:
	std::vector<ResourceInfo> _loadList;// ロード待ちリスト
	std::map<std::string, int> _handleMap;// 検索用マップ<名前、ハンドル>

	int _totalCnt = 0;// 総ロード数
};

