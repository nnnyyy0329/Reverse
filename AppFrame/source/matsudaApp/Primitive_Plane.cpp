#include "Primitive_Plane.h"

namespace {
	constexpr COLOR_U8 DEFAULT_DIFFUSE = { 255, 255, 255, 255 };
	constexpr COLOR_U8 DEFAULT_SPECULAR = { 0, 0, 0, 0 };
	constexpr VECTOR DEFAULT_NORMAL = { 0.0f, 1.0f, 0.0f };
}

Primitive_Plane::Primitive_Plane() {
}

Primitive_Plane::~Primitive_Plane() {
	_vertex.clear();
	_index.clear();
}

void Primitive_Plane::Render(){
	auto vertexNum = static_cast<int>(_vertex.size());
	auto indexNum = static_cast<int>(_index.size());

	auto polygonNum = indexNum / 3;// 三角形の数

	DrawPolygonIndexed3D(_vertex.data(), vertexNum, _index.data(), polygonNum, _grHandle, FALSE);
}

void Primitive_Plane::CreatePlane(int grHandle, const VECTOR& originPos, float tileSize, int tileX, int tileZ)
{
	_vertex.clear();
	_index.clear();

	_grHandle = grHandle;// テクスチャハンドルを保存

	// ステージサイズ
	auto stageWidth = tileSize * static_cast<float>(tileX);
	auto stageDepth = tileSize * static_cast<float>(tileZ);
	// ステージ中心が originPos になるように開始位置を計算
	auto startX = originPos.x - stageWidth * 0.5f;
	auto startZ = originPos.z - stageDepth * 0.5f;
	// 平面の基本の正方形用テーブル
	VECTOR pos0 = { 0.0f, 0.0f, 0.0f };
	VECTOR pos1 = { 0.0f, 0.0f, tileSize };
	VECTOR pos2 = { tileSize, 0.0f, 0.0f };
	VECTOR pos3 = { tileSize, 0.0f, tileSize };
	// 4頂点の相対座標をリストにもつ
	std::array<VECTOR, 4> posList = { pos0, pos1, pos2, pos3 };
	// uvのテーブル
	constexpr std::array<float, 4> uList = { 0.0f, 0.0f, 1.0f, 1.0f };
	constexpr std::array<float, 4> vList = { 0.0f, 1.0f, 0.0f, 1.0f };
	unsigned short offsetIndex = 0;
	// z, x軸の二重ループで全タイルを配置
	for (auto z = 0; z < tileZ; z++) {
		auto offsetZ = startZ + tileSize * static_cast<float>(z);
		for (auto x = 0; x < tileX; x++) {
			auto offsetX = startX + tileSize * static_cast<float>(x);
			// 正方形を形成する
			for (auto i = 0; i < 4; i++) {
				VERTEX3D vertex;
				VECTOR base = posList[i];// 正方形の基準頂点(ローカル座標)
				// ワールド上の位置へ移動
				base.x += offsetX;
				base.z += offsetZ;

				vertex.pos = base;// 頂点座標
				vertex.norm = DEFAULT_NORMAL;
				vertex.dif = DEFAULT_DIFFUSE;
				vertex.spc = DEFAULT_SPECULAR;
				vertex.u = uList[i];
				vertex.v = vList[i];
				// 頂点リストに追加
				_vertex.push_back(vertex);
			}
			// 正方形を２つの三角形で描くためのインデックス
			// 頂点の順番は時計回り
			_index.push_back(offsetIndex);
			_index.push_back(offsetIndex + 1);
			_index.push_back(offsetIndex + 2);

			_index.push_back(offsetIndex + 2);
			_index.push_back(offsetIndex + 1);
			_index.push_back(offsetIndex + 3);

			offsetIndex += 4;// 次の正方形へ(頂点を４ずつ追加するから)
		}
	}
}
