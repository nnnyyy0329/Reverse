#pragma once
#include "appframe.h"

class ModeGame;
class StageBase;

// ステージ切り替え時のモード
class ModeStageChange : public ModeBase
{
	typedef ModeBase base;


public:
	ModeStageChange(ModeGame* modegGame, int nextStgeNum);
	virtual ~ModeStageChange();

	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;

private:
	ModeGame* _modeGame;// 参照するModeGame
	int _nextStageNum;// 次のステージ番号
	int _loadingProgress;// ロード進行度(0-100)
	bool _bIsLoadComplete;// ロード完了フラグ
	int _loadingTimer;// ロードタイマー

	std::shared_ptr<StageBase> _newStage;// 新しいステージ

	enum class LOADING_PHASE
	{
		START,// 開始
		LOADING,// ロード中
		COMPLETE// 完了
	};

	LOADING_PHASE _loadingPhase;
};

