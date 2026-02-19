#include "ModeTitle.h"
#include "ModeLoading.h"
#include "ApplicationMain.h"

bool ModeTitle::Initialize() 
{
	if (!base::Initialize()) { return false; }
	_bIsAddLoading = false;
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
	DrawFormatString(640, 300, GetColor(255, 255, 255), "===== TITLE =====");
	DrawFormatString(640, 340, GetColor(255, 255, 255), "Press Space");

	return true;
}
