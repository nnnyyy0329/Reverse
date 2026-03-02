#pragma once
#include "appframe.h"
#include "AbilityBase.h" 

// 選択状態を表す列挙型を追加
enum class SelectionState
{
	NONE,
	NOT_SELECTION,		// 非選択
	SELECTING,			// 選択中
	SELECT_COMPLETED,	// 選択確定
	COMPLETED,			// 選択処理完了
	_EOT_,
};

class PlayerManager;
class PlayerUnlockManager;

class AbilitySelectScreen
{
public:
	AbilitySelectScreen();
	virtual ~AbilitySelectScreen();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	// 選択関係関数
	void SelectScreenByInput();				// 入力による画面表示
	void SelectionByInput();				// 入力による選択処理
	void SelectRender();					// 選択要素の表示
	void SelectFrameRender();				// 選択画面表示
	void ResetSelection();					// 選択をリセットする関数を追加
	void CompleteSelection();				// 選択を完了する関数
	bool IsSelectActiveByPlayerType()const;	// プレイヤータイプに応じて選択可能かどうか

	// ゲッターセッター
	ABILITY_TYPE GetSelectedAbility()const	// 選択されたアビリティ取得
	{
		return ConvertSelectionToAbility(_iSelectedAbility);
	}
	int GetSelectedAbilityIndex()const { return _iSelectedAbility; }	// 選択されたアビリティ番号取得
	bool GetIsSelectComplete()const{ return _bIsSelectComplete; }		// 選択完了フラグ取得
	bool GetIsScreenActive()const { return _bIsScreenActive; }			// 選択画面表示フラグ取得
	SelectionState GetSelectionState()const{ return _selectionState; }

	// クラスセット
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager) { _playerManager = playerManager; }
	void SetPlayerUnlockManager(std::shared_ptr<PlayerUnlockManager> unlockManager) { _playerUnlockManager = unlockManager; }

private:

protected:
	std::shared_ptr<PlayerManager> _playerManager;				// プレイヤーマネージャー
	std::shared_ptr<PlayerUnlockManager> _playerUnlockManager;	// プレイヤーアンロックマネージャー

	// 表示用ハンドル
	int _iHandle1;
	int _iHandle2;
	int _iHandle3;

	// 選択状態の管理
	SelectionState _selectionState;	// 選択状態
	int _iCurrentSelection;			// 現在選択中のアビリティ
	int _iSelectedAbility;			// 決定されたアビリティ

	bool _bIsSelectComplete;	// 選択完了フラグ
	bool _bIsScreenActive;		// 選択画面表示フラグ

	// 選択カーソル用
	int _iCursorCount;
	bool _bShowCursor;
};

