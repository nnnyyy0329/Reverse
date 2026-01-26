#pragma once
#include "appframe.h"

class Enemy;

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

	// 敵の座標設定情報
	struct ENEMYPOS
	{
		std::string typeName;// 敵の種類
		VECTOR vPos;// 位置
		VECTOR vRot;// 回転
	};

	StageBase(int stageNum);
	virtual ~StageBase();

	virtual void Process();
	virtual void Render();
	virtual void DebugRender();

	const std::vector<std::shared_ptr<Enemy>>& GetEnemies() const { return _stageEnemies; }// ステージ内の敵リストを取得
	const std::vector<MODELPOS>& GetMapModelPosList() const { return _mapModelPosList; }
	//int GetHandleMap() const { return _handleMap; }// マップモデルのハンドルを取得
	//int GetFrameMapCollision() const { return _frameMapCollision; }// マップのコリジョンフレームを取得

	// 敵の残数管理
	int GetTotalEnemyCnt() { return _totalEnemyCnt; }// ステージ内の敵の総数を取得
	int GetCurrentEnemyCnt() { return static_cast<int>(_stageEnemies.size()); }// 現在の敵の数を取得
	bool IsAllEnemiesDefeated() { return _stageEnemies.empty() && _totalEnemyCnt > 0; }// すべての敵が倒されたか

protected:
	std::map<std::string, int> _mapModelHandle;// マップモデル用ハンドル(名前、モデルハンドル)
	std::vector<std::shared_ptr<Enemy>> _stageEnemies;// ステージ内の敵リスト

	std::vector<MODELPOS> _mapModelPosList;// json読み込みで設定したマップモデルのリスト

	int _stageNum;// ステージ番号

	// テスト用マップ
	//int _handleMap;
	//int _handleSkySphere;
	//int _frameMapCollision;

	int _totalEnemyCnt;// ステージ内の敵の総数
};

