#include "StageBase.h"

#include <nlohmann/json.hpp>
#include <fstream>

StageBase::StageBase()
{
	// jsonファイルの読み込み(マップ)
	{
#if 0	// ブロック
		std::string path = "res/BoxFieldUE/";
		std::string jsonFile = "Stage.json";
		std::string jsonObjName = "Stage";
#endif
#if 0	// 浮島
		std::string path = "res/FloatingIsland/";
		std::string jsonFile = "FloatingIsland.json";
		std::string jsonObjName = "Stage";
#endif
#if 1	// 孤島
		std::string path = "res/IslandJson/";
		std::string jsonFile = "Island.json";
		std::string jsonObjName = "Island";
#endif

		std::ifstream file(path + jsonFile);
		nlohmann::json json;
		file >> json;
		nlohmann::json stage = json.at(jsonObjName);

		for (auto& data : stage) {
			MODELPOS modelPos;
			data.at("objectName").get_to(modelPos.name);
			// UEは左手座標系/Zup →左手座標系/Yup に変換しつつ取得
			data.at("translate").at("x").get_to(modelPos.pos.x);
			data.at("translate").at("z").get_to(modelPos.pos.y);
			data.at("translate").at("y").get_to(modelPos.pos.z);
			modelPos.pos.z *= -1.0f;// 座標の変換

			data.at("rotate").at("x").get_to(modelPos.rot.x);
			data.at("rotate").at("z").get_to(modelPos.rot.y);
			data.at("rotate").at("y").get_to(modelPos.rot.z);
			modelPos.rot.x = modelPos.rot.x * DEGREE_TO_RADIAN;// 回転はdegree→radianに
			modelPos.rot.y = modelPos.rot.y * DEGREE_TO_RADIAN;
			modelPos.rot.z = modelPos.rot.z * DEGREE_TO_RADIAN;

			data.at("scale").at("x").get_to(modelPos.scale.x);
			data.at("scale").at("z").get_to(modelPos.scale.y);
			data.at("scale").at("y").get_to(modelPos.scale.z);

			// 名前のモデルがすでに読み込み済みか？
			if (_mapModelHandle.count(modelPos.name) == 0) {
				// まだ読み込まれていないので読み込む
				std::string fileName = path + modelPos.name + ".mv1";
				int handle = MV1LoadModel(fileName.c_str());
				_mapModelHandle[modelPos.name] = handle;

				// コリジョン情報の生成
				int frameCollision = MV1SearchFrame(handle, ("UCX_" + modelPos.name).c_str());
				MV1SetupCollInfo(handle, frameCollision, 16, 16, 16);
			}
			// 名前から使うモデルハンドルを決める
			if (_mapModelHandle.count(modelPos.name) > 0) {
				modelPos.modelHandle = _mapModelHandle[modelPos.name];
				modelPos.drawFrame = MV1SearchFrame(modelPos.modelHandle, modelPos.name.c_str());
			}
			// データをリストに追加
			if (modelPos.modelHandle != -1) {
				_mapModelPosList.push_back(modelPos);
			}
		}
	}

	// jsonファイルの読み込み(敵)
	{
	}

	// テスト用マップ
//	{
//		_handleSkySphere = MV1LoadModel("res/base/SkySphere/skysphere.mv1");
//#if 1
//		// ダンジョン
//		_handleMap = MV1LoadModel("res/base/Dungeon/Dungeon.mv1");
//		_frameMapCollision = MV1SearchFrame(_handleMap, "dungeon_collision");
//#else
//		// フィールド
//		_handleMap = MV1LoadModel("res/base/Ground/Ground.mv1");
//		_frameMapCollision = MV1SearchFrame(_handleMap, "ground_navmesh");
//#endif
//		// コリジョン情報の生成
//		MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
//		// コリジョンのフレームを描画しない設定
//		MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
//	}

}

StageBase::~StageBase()
{
}

void StageBase::Process()
{
}

void StageBase::Render()
{
	// マップモデルの描画
	{
		for (auto ite = _mapModelPosList.begin(); ite != _mapModelPosList.end(); ++ite) {
			MV1SetPosition(ite->modelHandle, ite->pos);
			MV1SetRotationXYZ(ite->modelHandle, ite->rot);
			MV1SetScale(ite->modelHandle, ite->scale);
			MV1DrawFrame(ite->modelHandle, ite->drawFrame);
		}
	}

	// 敵の描画
	{
	}

	// テスト用マップの描画
	//{
	//	MV1DrawModel(_handleMap);
	//	MV1DrawModel(_handleSkySphere);
	//}
}
