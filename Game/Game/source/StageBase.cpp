#include "StageBase.h"
#include "Enemy.h"
#include "EnemyFactory.h"

#include <nlohmann/json.hpp>
#include <fstream>

StageBase::StageBase(int stageNum) 
	: _stageNum(stageNum)
	, _totalEnemyCnt(0)
{
	std::string path, jsonFile, jsonObjName;
	switch (_stageNum) {
	case 1:
		path = "res/stage/"; jsonFile = "try_stage_0.json"; jsonObjName = "res";
		break;
	case 2:
		path = "res/stage/"; jsonFile = "try_stage_1.json"; jsonObjName = "res";
		break;
	}

	// jsonファイルの読み込み(マップ)
	{
		LoadStageDataFromJson(
			path + jsonFile,
			jsonObjName,
			[&](const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)
			{
				MODELPOS modelPos;
				modelPos.name = name;
				modelPos.pos = pos;
				modelPos.rot = rot;
				modelPos.scale = scale;

				if (_mapModelHandle.count(name) == 0)
				{
					std::string fileName = path + name + ".mv1";
					_mapModelHandle[name] = MV1LoadModel(fileName.c_str());
				}

				if (_mapModelHandle.count(name) > 0)
				{
					modelPos.modelHandle = MV1DuplicateModel(_mapModelHandle[name]);
					modelPos.drawFrame = MV1SearchFrame(modelPos.modelHandle, name.c_str());

					std::string collisionName = "UCX_" + name;
					modelPos.collisionFrame = MV1SearchFrame(modelPos.modelHandle, collisionName.c_str());

					MV1SetPosition(modelPos.modelHandle, modelPos.pos);
					MV1SetRotationXYZ(modelPos.modelHandle, modelPos.rot);
					MV1SetScale(modelPos.modelHandle, modelPos.scale);

					if (modelPos.collisionFrame != -1)
					{
						MV1SetupCollInfo(modelPos.modelHandle, modelPos.collisionFrame, 8, 8, 8);
					}
				}

				if (modelPos.modelHandle != -1)
				{
					_mapModelPosList.push_back(modelPos);
				}
			}
		);
	}

	// jsonファイルの読み込み(敵)
	{
		std::string enemyObjName = "enemymarker";

		LoadStageDataFromJson(
			path + jsonFile,
			enemyObjName,
			[&](const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)
			{
				// 名前に応じて敵を生成
				if (name == "S_MarkerA")
				{
					_stageEnemies.push_back(
						EnemyFactory::CreateEnemy(EnemyType::MELEE, pos)
					);
					_totalEnemyCnt++;// 敵を追加したらカウントアップ
				}
				else if (name == "S_MarkerB")
				{
					_stageEnemies.push_back(
						EnemyFactory::CreateEnemy(EnemyType::RANGED, pos)
					);
					_totalEnemyCnt++;
				}
				else if (name == "S_MarkerC")
				{
					_stageEnemies.push_back(
						EnemyFactory::CreateEnemy(EnemyType::TANK, pos)
					);
					_totalEnemyCnt++;
				}
			}
		);





		//_stageEnemies.push_back(
		//	EnemyFactory::CreateEnemy(EnemyType::MELEE, VGet(80, 0.0f, -300.0f))// テストで調整
		//);
		//_totalEnemyCnt++;// 敵を追加したらカウントアップ

		//_stageEnemies.push_back(
		//	EnemyFactory::CreateEnemy(EnemyType::TANK, VGet(80.0f, 0.0f, -300.0f))
		//);
		//_totalEnemyCnt++;
	}

	// トリガー
	{
		std::string trigObjName = "portalmarker";
		
		LoadStageDataFromJson(
			path + jsonFile,
			trigObjName,
			[&](const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)
			{
				TRIGGERPOS trig;
				trig.name = name;
				trig.vPos = pos;
				trig.vRot = rot;
				trig.vScale = scale;

				if (_mapModelHandle.count(name) == 0)
				{
					std::string fileName = path + name + ".mv1";
					int handle = MV1LoadModel(fileName.c_str());
					_mapModelHandle[name] = handle;
				}

				if (_mapModelHandle.count(name) > 0)
				{
					trig.modelHandle = MV1DuplicateModel(_mapModelHandle[name]);

					std::string collisionName = "UCX_" + name;
					trig.collisionFrame = MV1SearchFrame(trig.modelHandle, collisionName.c_str());

					MV1SetPosition(trig.modelHandle, trig.vPos);
					MV1SetRotationXYZ(trig.modelHandle, trig.vRot);
					MV1SetScale(trig.modelHandle, trig.vScale);

					if (trig.collisionFrame != -1)
					{
						MV1SetupCollInfo(trig.modelHandle, trig.collisionFrame, 4, 4, 4);
					}
				}

				if (trig.modelHandle != -1)
				{
					_triggerList.push_back(trig);
				}
			}
		);
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
		// 敵の更新と削除処理
		for (auto it = _stageEnemies.begin(); it != _stageEnemies.end(); ) 
		{
			std::shared_ptr<Enemy> enemy = *it;

			enemy->Process();

			// 削除可能ならリストから削除
			if (enemy->CanRemove()) 
			{
				it = _stageEnemies.erase(it);
			}
			else 
			{
				++it;
			}
		}
	}
}

void StageBase::Render()
{
	// マップモデルの描画
	{
		for (auto ite = _mapModelPosList.begin(); ite != _mapModelPosList.end(); ++ite) {
			MV1DrawModel(ite->modelHandle);
			//MV1DrawFrame(ite->modelHandle, ite->drawFrame);
			//MV1DrawFrame(ite->modelHandle, ite->collisionFrame);// コリジョンフレームの描画
		}
	}

	// 敵の描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->Render();
		}
	}
}

void StageBase::DebugRender()
{
	// 敵のデバッグ情報描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->DebugRender();
		}
	}

	// 敵の残数をデバッグ表示
	{
		int x = 10;
		int y = 10;
		int size = 20;

		DrawFormatString(x, y, GetColor(255, 255, 0), "敵総数 : %d", _totalEnemyCnt); y += size;
		DrawFormatString(x, y, GetColor(255, 255, 0), "残り敵数 : %d", GetCurrentEnemyCnt()); y += size;
		DrawFormatString(x, y, GetColor(255, 255, 0), "全滅判定 : %s", IsAllEnemiesDefeated() ? "True" : "False"); y += size;
	}

	// トリガー情報
	{
		for (auto& trig : _triggerList) {
			MV1DrawModel(trig.modelHandle);
		}
	}
}

void StageBase::CollisionRender()
{
	// マップモデルのコリジョン描画
	{
		for (auto ite = _mapModelPosList.begin(); ite != _mapModelPosList.end(); ++ite) {
			MV1DrawFrame(ite->modelHandle, ite->collisionFrame);
		}
	}

	// 敵のコリジョン描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->CollisionRender();
		}
	}
}

void StageBase::LoadStageDataFromJson(
	const std::string& filePath,
	const std::string& objName,
	std::function<void(const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)> onLoadItem
)
{
	// ファイルを開く
	std::ifstream file(filePath);
	nlohmann::json json;
	file >> json;
	nlohmann::json stage = json.at(objName);

	// ループ処理
	for (auto& data : stage)
	{
		std::string name;
		VECTOR pos, rot, scale;

		// 名前取得
		data.at("objectName").get_to(name);

		// UEは左手座標系/Zup →左手座標系/Yup に変換しつつ取得
		data.at("translate").at("x").get_to(pos.x);
		data.at("translate").at("z").get_to(pos.y);
		data.at("translate").at("y").get_to(pos.z);
		pos.z *= -1.0f;// 座標の変換

		VECTOR rotDeg;
		data.at("rotate").at("x").get_to(rotDeg.x);
		data.at("rotate").at("z").get_to(rotDeg.y);
		data.at("rotate").at("y").get_to(rotDeg.z);
		rot.x = rotDeg.x * DEGREE_TO_RADIAN;// 回転はdegree→radianに
		rot.y = rotDeg.y * DEGREE_TO_RADIAN;
		rot.z = rotDeg.z * DEGREE_TO_RADIAN;

		data.at("scale").at("x").get_to(scale.x);
		data.at("scale").at("z").get_to(scale.y);
		data.at("scale").at("y").get_to(scale.z);

		// コールバック関数を呼び出す
		onLoadItem(name, pos, rot, scale);
	}
}

int StageBase::GetNextStageNumFromTrigger(const std::string& triggerName)const
{
	// トリガー名に応じて次のステージ番号を判定
	if (triggerName.find("portal_1") != std::string::npos)
	{
		return 1;
	}
	else if (triggerName.find("portal_0") != std::string::npos)
	{
		return 2;
	}
}
