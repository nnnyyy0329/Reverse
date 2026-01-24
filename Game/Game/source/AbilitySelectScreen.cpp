#include "AbilitySelectScreen.h"

AbilitySelectScreen::AbilitySelectScreen()
{
	_iHandle1 = ResourceServer::GetInstance()->GetHandle("select1");
	_iHandle2 = ResourceServer::GetInstance()->GetHandle("select2");
	_iHandle3 = ResourceServer::GetInstance()->GetHandle("select3");
}

AbilitySelectScreen::~AbilitySelectScreen()
{

}

bool AbilitySelectScreen::Initialize()
{
	return true;
}

bool AbilitySelectScreen::Terminate()
{
	return true;
}

bool AbilitySelectScreen::Process()
{
	return true;
}

bool AbilitySelectScreen::Render()
{
	int x = 0;
	int y = 0;
	int w = 1920;
	int h = 1080;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(x, y, x + w, y + h, GetColor(0, 0, 0), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	//DrawGraph(0, 0, _iHandle1, TRUE);
	DrawGraph(640, 0, _iHandle2, TRUE);
	DrawGraph(1280, 0, _iHandle3, TRUE);

	return true;
}
