#pragma once
#include "appframe.h"

class EnemyBase;

class StageBase
{
public:
	StageBase();
	virtual ~StageBase();

	virtual void Process();
	virtual void Render();

	const std::vector<std::shared_ptr<EnemyBase>>& GetEnemies() const { return _stageEnemies; }// ステージ内の敵リストを取得

protected:
	std::map<std::string, int> _mapModelHandle;// マップモデル用ハンドル(名前、モデルハンドル)
	std::vector<std::shared_ptr<EnemyBase>> _stageEnemies;// ステージ内の敵リスト

	// マップモデルの設定情報
	struct MODELPOS {
		std::string name;// 名前
		VECTOR pos;// 位置
		VECTOR rot;// 回転
		VECTOR scale;// スケール
		int modelHandle;// モデルハンドル
		int drawFrame;// 描画フレーム
	};
	std::vector<MODELPOS> _mapModelPosList;// json読み込みで設定したマップモデルのリスト

	int _stageNum;// ステージ番号


	// マップ用
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;

};

