#pragma once
#include "appframe.h"

class EnemyBase;

class StageBase
{
public:
	// マップモデルの設定情報
	struct MODELPOS {
		std::string name;// 名前
		VECTOR pos;// 位置
		VECTOR rot;// 回転
		VECTOR scale;// スケール
		int modelHandle;// モデルハンドル
		int drawFrame;// 描画フレーム
		int collisionFrame;// コリジョンフレーム
	};

	StageBase(int stageNum);
	virtual ~StageBase();

	virtual void Process();
	virtual void Render();
	virtual void DebugRender();

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const { return _stageEnemies; }// ステージ内の敵リストを取得
	const std::vector<MODELPOS>& GetMapModelPosList() const { return _mapModelPosList; }
	//int GetHandleMap() const { return _handleMap; }// マップモデルのハンドルを取得
	//int GetFrameMapCollision() const { return _frameMapCollision; }// マップのコリジョンフレームを取得

protected:
	std::map<std::string, int> _mapModelHandle;// マップモデル用ハンドル(名前、モデルハンドル)
	std::vector<std::shared_ptr<EnemyBase>> _stageEnemies;// ステージ内の敵リスト

	std::vector<MODELPOS> _mapModelPosList;// json読み込みで設定したマップモデルのリスト

	int _stageNum;// ステージ番号

	// テスト用マップ
	//int _handleMap;
	//int _handleSkySphere;
	//int _frameMapCollision;
};

