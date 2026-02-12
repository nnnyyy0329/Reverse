#include "ModeStageChange.h"
#include "ModeGame.h"
#include "StageBase.h"

ModeStageChange::ModeStageChange(ModeGame* modeGame, int nextStageNum)
	: _modeGame(modeGame)
	, _nextStageNum(nextStageNum)
	, _loadingProgress(0)
	, _bIsLoadComplete(false)
	, _loadingTimer(0)
	, _loadingPhase(LOADING_PHASE::START)
{
}

ModeStageChange::~ModeStageChange()
{
}

bool ModeStageChange::Initialize()
{
	if(!base::Initialize())
	{
		return false;
	}

	_loadingProgress = 0;
	_bIsLoadComplete = false;
	_loadingTimer = 0;
	_loadingPhase = LOADING_PHASE::START;
	_newStage = nullptr;

	return true;
}

bool ModeStageChange::Terminate()
{
	base::Terminate();

	return true;
}

bool ModeStageChange::Process()
{
	base::Process();

	// これより下のモードの処理をスキップ
	ModeServer::GetInstance()->SkipProcessUnderLayer();

	_loadingTimer++;

	switch (_loadingPhase)
	{
		case LOADING_PHASE::START:
			// 即座にロード開始
			_loadingPhase = LOADING_PHASE::LOADING;
			_loadingProgress = 0;
			break;

		case LOADING_PHASE::LOADING:
			// ステージのロード処理
			if (_loadingProgress < 30)
			{
				_loadingProgress += 10;
			}
			else if (_loadingProgress < 60)
			{
				// 新しいステージを生成
				if (_newStage == nullptr)
				{
					_newStage = std::make_shared<StageBase>(_nextStageNum);
				}
				_loadingProgress += 10;
			}
			else if (_loadingProgress < 90)
			{
				// ModeGameのステージを切り替え
				if (_modeGame && _newStage)
				{
					_modeGame->ChangeStage(_newStage, _nextStageNum);
					_newStage = nullptr;// 所有権を移す
				}
				_loadingProgress += 10;
			}
			else
			{
				// ロード完了
				_loadingProgress = 100;
				_bIsLoadComplete = true;
				_loadingPhase = LOADING_PHASE::COMPLETE;
			}
			break;

		case  LOADING_PHASE::COMPLETE:
			// ロード完了後、少し待ってから削除
			if (_loadingTimer > 120)
			{
				ModeServer::GetInstance()->Del(this);
			}
			break;
	}

	return true;
}

bool ModeStageChange::Render()
{
	base::Render();

	// これより下のモードの描画をスキップ
	ModeServer::GetInstance()->SkipRenderUnderLayer();

	DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);// 背景を黒で塗りつぶす
	DrawFormatString(900, 540, GetColor(255, 255, 255), "StageChange %d%%", _loadingProgress);

	return true;
}
