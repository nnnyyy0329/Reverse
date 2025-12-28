#include "StageBase.h"
#include "EnemyBase.h"

#include <nlohmann/json.hpp>
#include <fstream>

StageBase::StageBase(int stageNum) : _stageNum(stageNum)
{
	// jsonファイルの読み込み(マップ)
	{
		std::string path, jsonFile, jsonObjName;

		switch (_stageNum) {// ステージ番号で読み込むファイルを分ける
		case 1:
			// ブロック
			path = "res/try/";
			jsonFile = "try.json";
			jsonObjName = "Playground";
			break;
		case 2:
			// 浮島
			path = "res/FloatingIsland/";
			jsonFile = "FloatingIsland.json";
			jsonObjName = "Stage";
			break;
		case 3:
			// 孤島
			path = "res/IslandJson/";
			jsonFile = "Island.json";
			jsonObjName = "Island";
			break;
		}

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
				modelPos.collisionFrame = frameCollision;
			}
			// 名前から使うモデルハンドルを決める
			if (_mapModelHandle.count(modelPos.name) > 0) {
				// 既に読み込まれているモデルを複製して使う
				modelPos.modelHandle = MV1DuplicateModel(_mapModelHandle[modelPos.name]);
				modelPos.drawFrame = MV1SearchFrame(modelPos.modelHandle, modelPos.name.c_str());

				// モデルの情報をそれぞれに合わせる
				MV1SetPosition(modelPos.modelHandle, modelPos.pos);
				MV1SetRotationXYZ(modelPos.modelHandle, modelPos.rot);
				MV1SetScale(modelPos.modelHandle, modelPos.scale);
				MV1RefreshCollInfo(modelPos.modelHandle);// 行列が変わるためコリジョン情報を更新
			}
			// データをリストに追加
			if (modelPos.modelHandle != -1) {
				_mapModelPosList.push_back(modelPos);
			}
		}
	}

	// jsonファイルの読み込み(敵)
	{
		std::shared_ptr<EnemyBase> enTest = std::make_shared<EnemyBase>();
		_stageEnemies.push_back(enTest);
	}
}

StageBase::~StageBase()
{
}

void StageBase::Process()
{
	// マップモデルの更新
	{
	}

	// 敵の更新
	{
		for (auto& enemy : _stageEnemies) {
			enemy->Process();
		}
	}
}

void StageBase::Render()
{
	// マップモデルの描画
	{
		for (auto ite = _mapModelPosList.begin(); ite != _mapModelPosList.end(); ++ite) {
			MV1DrawFrame(ite->modelHandle, ite->drawFrame);
			//MV1DrawFrame(ite->modelHandle, ite->collisionFrame);// コリジョンフレームの描画
		}
	}

	// 敵の描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->Render();
		}
	}

	// テスト用マップの描画
	//{
	//	MV1DrawModel(_handleMap);
	//	MV1DrawModel(_handleSkySphere);
	//}
}

void StageBase::DebugRender()
{
	// 敵のデバッグ描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->DebugRender();
		}
	}
}
