#pragma once
#include "DxLib.h"

#include <vector>
#include <array>

class Primitive_Cube
{
public:
	Primitive_Cube();
	virtual ~Primitive_Cube();
	void Render();

	// ‘ÎŠpüã‚ÉˆÊ’u‚·‚é2“_‚ğw’è‚µ‚Ä—§•û‘Ì‚ğ¶¬
	void CreateCube(int grHandle, const VECTOR& p1, const VECTOR& p2);

protected:
	std::vector<VERTEX3D> _vertex;
	std::vector<unsigned short> _index;
	float _size;
	std::vector<std::array<VECTOR, 4>> _faceList;

	int _grHandle;
};

