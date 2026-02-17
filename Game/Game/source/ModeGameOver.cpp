#include "ModeGameOver.h"
#include "ModeGame.h"
#include "ApplicationMain.h"

ModeGameOver::ModeGameOver(ModeGame* modeGame)
	: _modeGame(modeGame)
{
}

ModeGameOver::~ModeGameOver()
{
}

bool ModeGameOver::Initialize()
{
	if (!base::Initialize()) { return false; }
	return true;
}

bool ModeGameOver::Terminate()
{
	base::Terminate();
	return true;
}

bool ModeGameOver::Process()
{
	base::Process();

	// これより下のモードの処理をスキップ
	ModeServer::GetInstance()->SkipProcessUnderLayer();

	int trg = ApplicationMain::GetInstance()->GetTrg();

	if (trg & PAD_INPUT_10) {// spaceキー
		// 現在のステージ番号からリスタート
		int currentStageNum = _modeGame->GetCurrentStageNum();
		_modeGame->RequestStageChange(currentStageNum);

		// このモードを削除
		ModeServer::GetInstance()->Del(this);
	}

	return true;
}

bool ModeGameOver::Render()
{
	base::Render();

	// 背景を暗くする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawFormatString(640, 300, GetColor(255, 0, 0), "===== GAME OVER =====");
	DrawFormatString(640, 340, GetColor(255, 255, 255), "Press Space to Restart");
	return true;
}
