#include "StageBase.h"
#include "Enemy.h"
#include "EnemyFactory.h"
#include "ModeGame.h"

#include <nlohmann/json.hpp>
#include <fstream>

StageBase::StageBase(int stageNum) 
	: _stageNum(stageNum)
	, _totalEnemyCnt(0)
	, _currentBGMName("")
	, _vPlayerStartPos(VGet(0.0f, 0.0f, 0.0f))
	, _vPlayerStartRot(VGet(0.0f, 0.0f, 0.0f))
{
	std::string path, jsonFile, jsonObjName;
	switch (_stageNum) 
	{
	case 1:
		path = "res/stage/json/"; jsonFile = "stage_01.json"; jsonObjName = "res";
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
					// 読み込み済みのモデルハンドルを取得
					int handle = ResourceServer::GetInstance()->GetHandle(name);
					if (handle != -1)
					{
						_mapModelHandle[name] = handle;
					}
					//else
					//{
					//	// 読み込まれていないなら直接ロード
					//	std::string fileName = path + name + ".mv1";
					//	_mapModelHandle[name] = MV1LoadModel(fileName.c_str());
					//}
				}

				if (_mapModelHandle.count(name) > 0)
				{
					modelPos.modelHandle = MV1DuplicateModel(_mapModelHandle[name]);
					modelPos.drawFrame = MV1SearchFrame(modelPos.modelHandle, name.c_str());

					std::string collisionName = "UCX_" + name;
					modelPos.collisionFrame = MV1SearchFrame(modelPos.modelHandle, name.c_str());

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
				if (name == "S_Enemy00")
				{
					_stageEnemies.push_back(
						EnemyFactory::CreateEnemy(EnemyType::NORMAL, pos, false)
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
					int handle = ResourceServer::GetInstance()->GetHandle(name);
					if (handle != -1)
					{
						_mapModelHandle[name] = handle;
					}
					//else
					//{
					//	std::string fileName = path + name + ".mv1";
					//	_mapModelHandle[name] = MV1LoadModel(fileName.c_str());
					//}
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





	// プレイヤー初期位置
	{
		std::string playerObjName = "playermarker";

		LoadStageDataFromJson(
			path + jsonFile,
			playerObjName,
			[&](const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)
			{
				if(name == "S_Player")
				{
					// 初期位置を保存
					_vPlayerStartPos = pos;
					_vPlayerStartRot = rot;

					// ModeGameのインスタンスを取得
					ModeGame* modeGame = (ModeGame*)ModeServer::GetInstance()->Get("game");
					if (modeGame != nullptr)
					{
						modeGame->SetPlayerConfig(pos, rot);
					}
				}
			}
		);
	}

	// 敵の移動可能範囲
	{
		std::string areaObjName = "Area";

		LoadStageDataFromJson(
			path + jsonFile,
			areaObjName,
			[&](const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)
			{
				MODELPOS area;
				area.name = name;
				area.pos = pos;
				area.rot = rot;
				area.scale = scale;

				if (_mapModelHandle.count(name) == 0)
				{
					int handle = ResourceServer::GetInstance()->GetHandle(name);
					if (handle != -1)
					{
						_mapModelHandle[name] = handle;
					}
					//else
					//{
					//	std::string fileName = path + name + ".mv1";
					//	_mapModelHandle[name] = MV1LoadModel(fileName.c_str());
					//}
				}

				if (_mapModelHandle.count(name) > 0)
				{
					area.modelHandle = MV1DuplicateModel(_mapModelHandle[name]);

					std::string collisionName = "UCX_" + name;
					area.collisionFrame = MV1SearchFrame(area.modelHandle, collisionName.c_str());

					MV1SetPosition(area.modelHandle, area.pos);
					MV1SetRotationXYZ(area.modelHandle, area.rot);
					MV1SetScale(area.modelHandle, area.scale);

					if (area.collisionFrame != -1)
					{
						MV1SetupCollInfo(area.modelHandle, area.collisionFrame, 4, 4, 4);
					}
				}

				if (area.modelHandle != -1)
				{
					_moveAreaList.push_back(area);
				}
			}
		);
	}

	// BGM
	switch (_stageNum)
	{
	case 1:
		_currentBGMName = "BGM_Stage01";
		break;
	case 2:
		_currentBGMName = "BGM_Stage02";
		break;
	}
	PlayStageBGM();

}

StageBase::~StageBase()
{
	StopStageBGM();
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
			//MV1DrawModel(ite->modelHandle);
			MV1DrawFrame(ite->modelHandle, ite->drawFrame);
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
}

void StageBase::CollisionRender()
{
	// マップモデルのコリジョン描画
	{
		for (auto ite = _mapModelPosList.begin(); ite != _mapModelPosList.end(); ++ite) {
			//MV1DrawFrame(ite->modelHandle, ite->collisionFrame);
		}
	}

	// 敵のコリジョン描画
	{
		for (auto& enemy : _stageEnemies) {
			enemy->CollisionRender();
		}
	}

	// トリガー情報
	{
		for (auto& trig : _triggerList) {
			MV1DrawModel(trig.modelHandle);
		}
	}

	// 敵の移動可能範囲
	{
		for (auto& area : _moveAreaList) {
			MV1DrawModel(area.modelHandle);
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

int StageBase::GetNextStageNumFromTrigger(const std::string& triggerName)
{
	// トリガー名に応じて次のステージ番号を判定
	if (triggerName.find("portal_1") != std::string::npos)
	{
		return 1;
	}
	else if (triggerName.find("S_Portal_0to1") != std::string::npos)
	{
		return 2;
	}
}

void StageBase::PlayStageBGM()
{
	if (_currentBGMName.empty()) return;

	// ループ再生
	auto bgmHandle = SoundServer::GetInstance()->Play(_currentBGMName, DX_PLAYTYPE_LOOP);

	// ボリューム設定
}

void StageBase::StopStageBGM()
{
	if (_currentBGMName.empty()) return;

	// BGMを停止
	SoundServer::GetInstance()->Stop(_currentBGMName);
}
