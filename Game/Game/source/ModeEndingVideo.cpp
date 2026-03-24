#include "ModeEndingVideo.h"
#include "ModeLogo.h"
#include "DxLib.h"

bool ModeEndingVideo::Initialize()
{
	if(!base::Initialize()) { return false; }

	_frameCount = 0;
	_started = false;
	_finished = false;
	_movieGraph = -1;

	// 動画ファイルのパスを必要に応じて変更してください
	const char* moviePath = "res/Ending/endroll.mp4";

	// PlayMovieToGraph を使って動画をグラフとして再生（第2引数: 0=ループしない）
	_movieGraph = LoadGraph(moviePath);
	if(_movieGraph >= 0)
	{
		PlayMovieToGraph(_movieGraph, 0);
	}
	if(_movieGraph < 0)
	{
		// ムービー再生に失敗したら即座にロゴへ遷移
		ModeServer::GetInstance()->Clear();
		ModeServer::GetInstance()->Add(new ModeLogo(), 100, "logo");
		return true;
	}

	_started = true;
	return true;
}

bool ModeEndingVideo::Terminate()
{
	// 念のため停止＆解放
	if(_movieGraph >= 0)
	{
		DeleteGraph(_movieGraph);
		_movieGraph = -1;
	}

	base::Terminate();
	return true;
}

bool ModeEndingVideo::Process()
{
	_frameCount++;

	// ユーザー操作でスキップ可能にする
	auto& im = InputManager::GetInstance();
	if(im.IsTrigger(INPUT_ACTION::SKIP))
	{
		_finished = true;
	}


	if(_finished)
	{
		// 停止と解放
		if(_movieGraph >= 0)
		{
			DeleteGraph(_movieGraph);
			_movieGraph = -1;
		}

		// 次モードへ（ロゴ）
		
		SoundServer::GetInstance()->Stop("BGM_Ending");
		ModeServer::GetInstance()->Clear();
		ModeServer::GetInstance()->Add(new ModeLogo(), 100, "logo");
		// 自分自身は Clear() のため削除不要（Clear() がすべて削除）
		return true;
	}

	return true;
}

bool ModeEndingVideo::Render()
{
	// 背景黒で動画をフルスクリーン描画
	ClearDrawScreen();
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);

	if(_movieGraph >= 0)
	{
		// グラフを画面サイズに拡張して描画
		DrawExtendGraph(0, 0, 1920, 1080, _movieGraph, TRUE);
	}
	else
	{
		// 動画がない場合は黒画面（短時間だけ表示）
	}

	return true;
}