#include "Item.h"

Item::Item()
{

}
Item::~Item()
{

}
bool Item::Initialize()
{
	return true;
}

bool Item::Terminate()
{
	return true;
}

bool Item::Process()
{
	return true;
}

bool Item::Render()
{
	DrawCapsule3D
	(
		_vPos,                   // Pos1：カプセルの端1の中心座標
		_vCollisionTop,          // Pos2：カプセルの端2の中心座標
		_fCollisionR,            // r：半径
		16,                      // DivNum：細かさ
		GetColor(255, 0, 0),     // DifColor：メインの色（赤）
		GetColor(255, 255, 255), // SpcColor：光が反射した時の色（白）
		TRUE                     // FillFlag：塗りつぶすかどうか（TRUEで塗りつぶし）
	);
	return true;
}

void Item::DebugRender()
{
}

void Item::CollisionRender()
{
}

void Item::CreateItems(VECTOR potision, float height ,float itemRadius)
{
	_vPos = potision;
	_vCollisionTop = VAdd(potision, VGet(0.0f, height, 0.0f));
	_fCollisionR = itemRadius;
}