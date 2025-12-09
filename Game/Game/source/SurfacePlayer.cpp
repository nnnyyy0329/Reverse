#include "SurfacePlayer.h"

SurfacePlayer::SurfacePlayer()
{
	_iHandle = MV1LoadModel("res/base/SDChar/SDChar.mv1");
}

SurfacePlayer::~SurfacePlayer()
{
	MV1DeleteModel(_iHandle);
}

bool SurfacePlayer::Initialize()
{
	return true;
}

bool SurfacePlayer::Terminate()
{
	return true;
}

bool SurfacePlayer::Process()
{
	return true;
}

bool SurfacePlayer::Render()
{
	return true;
}