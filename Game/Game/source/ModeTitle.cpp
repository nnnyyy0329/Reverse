#include "ModeTitle.h"
#include "ModeLoading.h"
#include "ApplicationMain.h"



bool ModeTitle::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddLoading = false;

	_titleHandle = LoadGraph("res/Graph/Title.png");
	_alpha = 0;
	_fadeState = 0; // 0:フェードイン, 1:表示, 2:フェードアウト
	_frameCount = 0;
	return true;
}

bool ModeTitle::Terminate() 
{
	base::Terminate();
	


	return true;
}

bool ModeTitle::Process() 
{
	int trg = ApplicationMain::GetInstance()->GetTrg();

	

	if (!_bIsAddLoading && trg & PAD_INPUT_10) {
		_bIsAddLoading = true;
		ModeServer::GetInstance()->Add(new ModeLoading(), 9999, "loading");
		ModeServer::GetInstance()->Del(this);
	}

	return true;
}

bool ModeTitle::Render()
{
	// 背景を黒でクリア
	ClearDrawScreen();


	// 画像サイズを取得
	int w, h;
	GetGraphSize(_titleHandle, &w, &h);

	// スケールを小さくする（0.3倍〜0.5倍程度）
	float scale = 0.5f;  // この値を調整してサイズを変更
	int scaledW = static_cast<int>(w * scale);
	int scaledH = static_cast<int>(h * scale);

	// 画面中央に配置
	int x = (1920 - scaledW) / 2;
	int y = (1080 - scaledH) / 2;

	// 拡縮描画
	DrawExtendGraph(x, y, x + scaledW, y + scaledH, _titleHandle, TRUE);

	// デバッグ表示
	DrawFormatString(640, 340, GetColor(255, 255, 255), "Press Space or Button");

	return true;
}