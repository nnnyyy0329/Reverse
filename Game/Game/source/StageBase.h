#pragma once
#include "appframe.h"

class Enemy;

class StageBase
{
public:
	// マップモデルの設定情報
	struct MODELPOS 
	{
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

	// トリガーの設定情報
	struct TRIGGERPOS
	{
		std::string name;// トリガー名
		VECTOR vPos;// 位置
		VECTOR vRot;// 回転
		VECTOR vScale;// スケール
		int modelHandle;// モデルハンドル
		int drawFrame;// 描画フレーム
		int collisionFrame;// コリジョンフレーム
	};

	StageBase(int stageNum);
	virtual ~StageBase();

	virtual void Process();
	virtual void Render();
	virtual void DebugRender();
	virtual void CollisionRender();

	const std::vector<std::shared_ptr<Enemy>>& GetEnemies() const { return _stageEnemies; }
	const std::vector<MODELPOS>& GetMapModelPosList() const { return _mapModelPosList; }
	const std::vector<TRIGGERPOS>& GetTriggerList() const { return _triggerList; }

	// ステージ切り替え
	int GetNextStageNumFromTrigger(const std::string& triggerName);// トリガー名から次のステージ番号を取得

	// jsonファイルからステージデータを読み込む
	void LoadStageDataFromJson(
		const std::string& filePath,
		const std::string& objName,
		std::function<void(const std::string& name, const VECTOR& pos, const VECTOR& rot, const VECTOR& scale)> onLoadItem
	);

	// 敵の残数管理
	int GetTotalEnemyCnt() { return _totalEnemyCnt; }// ステージ内の敵の総数を取得
	int GetCurrentEnemyCnt() { return static_cast<int>(_stageEnemies.size()); }// 現在の敵の数を取得
	bool IsAllEnemiesDefeated() { return _stageEnemies.empty() && _totalEnemyCnt > 0; }// すべての敵が倒されたか

	// BGM関連
	void PlayStageBGM();// ステージBGMを再生
	void StopStageBGM();// ステージBGMを停止
	std::string GetCurrentBGMName() { return _currentBGMName; }// 現在のBGM名を取得

protected:
	std::map<std::string, int> _mapModelHandle;// マップモデル用ハンドル(名前、モデルハンドル)
	std::vector<std::shared_ptr<Enemy>> _stageEnemies;// ステージ内の敵リスト

	std::vector<MODELPOS> _mapModelPosList;// json読み込みで設定したマップモデルのリスト

	int _stageNum;// ステージ番号

	int _totalEnemyCnt;// ステージ内の敵の総数

	std::vector<TRIGGERPOS> _triggerList;// トリガーリスト

	// BGM関連
	std::string _currentBGMName;// 再生中のBGM名

};

