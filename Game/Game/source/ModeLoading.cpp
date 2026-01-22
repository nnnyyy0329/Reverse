#include "ModeLoading.h"
#include "ModeGame.h"

bool ModeLoading::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddGame = false;
	// リソースのロード開始
	ResourceServer::GetInstance()->StartLoadAsync();
	return true;
}

bool ModeLoading::Terminate() 
{
	base::Terminate();
	return true;
}

bool ModeLoading::Process()
{
	// ロード進捗の確認
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();

	// ロードが完了した後の処理をここに追加
	if (!_bIsAddGame && ResourceServer::GetInstance()->IsLoadComplete()) {
		_bIsAddGame = true;
		// モードゲームの登録
		ModeServer::GetInstance()->Add(new ModeGame(), 1, "game");
	}

	return true;
}

bool ModeLoading::Render()
{
	auto progress = ResourceServer::GetInstance()->GetLoadProgress();

	// 進捗率を描画

	return true;
}