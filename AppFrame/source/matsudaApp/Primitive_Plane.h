#pragma once
#include "DxLib.h"

#include <vector>
#include <array>

class Primitive_Plane
{
public:
	Primitive_Plane();
	virtual ~Primitive_Plane();
	void Render();

	// テクスチャハンドル、原点位置、タイルサイズ、分割数を指定して平面を生成
	void CreatePlane(int grHandle, const VECTOR& originPos, float tileSize, int tileX, int tileZ);

protected:
	std::vector<VERTEX3D> _vertex;
	std::vector<unsigned short> _index;

	int _grHandle;
};

