#pragma once
#include <map>
#include <string>
#include "DxLib.h"
#include "EffekseerForDXLib.h"

class EffectServer
{
public:
	// シングルトンインスタンス取得
	static EffectServer* GetInstance();

	void Initialize();// 初期化
	void Terminate();// 終了
	void Update();// 更新
	void Render();// 描画

	// エフェクトリソースの読み込み
	// 呼び出すときの名前、ファイルパス、読み込むときのスケール
	bool Load(const std::string& name, const char* path, float fScale = 1.0f);

	// エフェクトの再生
	// 登録した名前、座標
	// 戻り値：再生中のエフェクトハンドル
	int Play(const std::string& name, VECTOR vPos);

	// 再生中のエフェクトの停止
	void Stop(int playingHandle);

	// 再生中のエフェクトの位置更新
	void SetPos(int playingHandle, VECTOR vPos);
	// 回転の更新(ラジアン)
	void SetRot(int playingHandle, VECTOR vRot);

private:
	// コンストラクタ・デストラクタの隠蔽
	EffectServer() {}
	~EffectServer() {}
	// コピーを禁止
	EffectServer(const EffectServer&) = delete;
	EffectServer& operator=(const EffectServer&) = delete;

	// リソース管理用マップ<名前、エフェクトハンドル>
	std::map<std::string, int> _resources;
};
