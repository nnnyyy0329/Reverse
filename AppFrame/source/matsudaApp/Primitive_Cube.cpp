#include "Primitive_Cube.h"
#include "MyMath.h"

#include <algorithm>

namespace {
	constexpr auto DEFAULT_SIZE = 100.0;
	constexpr COLOR_U8 DEFAULT_DIFFUSE = { 255, 255, 255, 255 };
	constexpr COLOR_U8 DEFAULT_SPECULAR = { 0, 0, 0, 0 };

	VECTOR NormalFront = { 0.0f, 0.0f, -1.0f };
	VECTOR NormalRight = { 1.0f, 0.0f, 0.0f };
	VECTOR NormalBack = { 0.0f, 0.0f, 1.0f };
	VECTOR NormalLeft = { -1.0f, 0.0f, 0.0f };
	VECTOR NormalTop = { 0.0f, 1.0f, 0.0f };
	VECTOR NormalBottom = { 0.0f, -1.0f, 0.0f };
	std::array<VECTOR, 6> normalList = {
		NormalFront,
		NormalRight,
		NormalBack,
		NormalLeft,
		NormalTop,
		NormalBottom
	};
}

Primitive_Cube::Primitive_Cube() {
	_size = DEFAULT_SIZE;
}

Primitive_Cube::~Primitive_Cube() {
	_vertex.clear();
	_index.clear();
	_faceList.clear();
}

void Primitive_Cube::Render() {
	auto vertexNum = static_cast<int>(_vertex.size());
	auto indexNum = static_cast<int>(_index.size());

	auto polygonNum = indexNum / 3;// 三角形の数

	DrawPolygonIndexed3D(_vertex.data(), vertexNum, _index.data(), polygonNum, _grHandle, FALSE);
}

// 対角線上に位置する2点を指定して立方体を生成
void Primitive_Cube::CreateCube(int grHandle, const VECTOR& p1, const VECTOR& p2) {
	_vertex.clear();
	_index.clear();
	_faceList.clear();

	_grHandle = grHandle;// テクスチャハンドルを保存

	// p1, p2 は対角線上の2点とする
	// 各軸ごとにmin, maxを計算する
	float minX = (std::min)(p1.x, p2.x);
	float maxX = (std::max)(p1.x, p2.x);
	float minY = (std::min)(p1.y, p2.y);
	float maxY = (std::max)(p1.y, p2.y);
	float minZ = (std::min)(p1.z, p2.z);
	float maxZ = (std::max)(p1.z, p2.z);

	// 8頂点の位置を計算する
	// 上面(maxY)と下面(minY)のそれぞれで、左右(X)、前後(Z)の組み合わせで座標を設定
	VECTOR pos0 = { minX, maxY, minZ };// 上面 左手前
	VECTOR pos1 = { minX, maxY, maxZ };// 上面 左奥
	VECTOR pos2 = { maxX, maxY, minZ };// 上面 右手前
	VECTOR pos3 = { maxX, maxY, maxZ };// 上面 右奥
	VECTOR pos4 = { minX, minY, minZ };// 下面 左手前
	VECTOR pos5 = { minX, minY, maxZ };// 下面 左奥
	VECTOR pos6 = { maxX, minY, minZ };// 下面 右手前
	VECTOR pos7 = { maxX, minY, maxZ };// 下面 右奥

	// 6つの面を4頂点で定義する
	std::array<VECTOR, 4> faceFront = { pos4, pos0, pos6, pos2 };// 前面
	std::array<VECTOR, 4> faceRight = { pos6, pos2, pos7, pos3 };// 右面
	std::array<VECTOR, 4> faceBack = { pos7, pos3, pos5, pos1 };// 背面
	std::array<VECTOR, 4> faceLeft = { pos5, pos1, pos4, pos0 };// 左面
	std::array<VECTOR, 4> faceTop = { pos0, pos1, pos2, pos3 };// 上面
	std::array<VECTOR, 4> faceBottom = { pos4, pos5, pos6, pos7 };// 下面

	_faceList.push_back(faceFront);
	_faceList.push_back(faceRight);
	_faceList.push_back(faceBack);
	_faceList.push_back(faceLeft);
	_faceList.push_back(faceTop);
	_faceList.push_back(faceBottom);

	constexpr std::array<float, 4> uList = { 0.0f, 0.0f, 1.0f, 1.0f };
	constexpr std::array<float, 4> vList = { 0.0f, 1.0f, 0.0f, 1.0f };

	unsigned short offsetIndex = 0;

	// 6面分の頂点とインデックスを作成
	for (int face = 0; face < 6; ++face) {
		for (int i = 0; i < 4; ++i) {
			VERTEX3D vertex;
			auto faceVertex = _faceList[face];

			vertex.pos = faceVertex[i];
			vertex.norm = normalList[face];
			vertex.dif = DEFAULT_DIFFUSE;
			vertex.spc = DEFAULT_SPECULAR;
			vertex.u = uList[i];
			vertex.v = vList[i];

			_vertex.push_back(std::move(vertex));
		}

		_index.push_back(offsetIndex);
		_index.push_back(offsetIndex + 1);
		_index.push_back(offsetIndex + 2);

		_index.push_back(offsetIndex + 2);
		_index.push_back(offsetIndex + 1);
		_index.push_back(offsetIndex + 3);

		offsetIndex += 4;
	}
}