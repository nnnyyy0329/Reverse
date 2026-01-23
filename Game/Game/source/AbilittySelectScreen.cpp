#include "AbilittySelectScreen.h"

AbilittySelectScreen::AbilittySelectScreen()
{
	_iHandle1 = ResourceServer::GetInstance()->GetHandle("select1");
	_iHandle2 = ResourceServer::GetInstance()->GetHandle("select2");
	_iHandle3 = ResourceServer::GetInstance()->GetHandle("select3");

	_drawX = 0;
	_drawY = 0;
}

AbilittySelectScreen::~AbilittySelectScreen()
{

}

bool AbilittySelectScreen::Initialize()
{
	return true;
}

bool AbilittySelectScreen::Terminate()
{
	return true;
}

bool AbilittySelectScreen::Process()
{
	return true;
}

bool AbilittySelectScreen::Render()
{
	for(_drawX = 0; _drawX <= 1920; _drawX += 640)
	{
		for(_drawY = 0; _drawY <= 720; _drawY += 360){
			DrawGraph(_drawX, _drawY, _iHandle1, TRUE);
			DrawGraph(_drawX, _drawY, _iHandle2, TRUE);
			DrawGraph(_drawX, _drawY, _iHandle3, TRUE);
		}
	}

	return true;
}
