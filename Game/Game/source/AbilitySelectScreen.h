#pragma once
#include "appframe.h"
#include "AbilityBase.h" 

/// @brief 選択状態を表す列挙型
enum class SelectionState
{
	NONE,
	NOT_SELECTION,		// 非選択
	SELECTING,			// 選択中
	SELECT_COMPLETED,	// 選択確定
	COMPLETED,			// 選択処理完了
	_EOT_,
};

/// @brief 前方宣言
class PlayerManager;
class PlayerUnlockManager;

/// @brief 能力選択画面クラス
class AbilitySelectScreen
{
public:
	AbilitySelectScreen();
	virtual ~AbilitySelectScreen();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();


	/* 選択関係関数 */
	
	/// @brief 入力に応じて選択画面の表示を切り替える関数です。
	void SelectScreenByInput();		
	
	/// @brief 入力に応じて選択処理を行う関数です。
	void SelectionByInput();		
	
	/// @brief 選択要素の表示関数です。
	void SelectRender();			
	
	/// @brief 選択画面の表示関数です。
	void SelectFrameRender();		
	
	/// @brief 選択をリセットする関数です。
	void ResetSelection();			
	
	/// @brief 選択を完了する関数です。
	void CompleteSelection();	
	
	/// @brief プレイヤータイプに応じて選択可能かどうかを判断する関数です。
	bool IsSelectActiveByPlayerType()const;	


	/* ゲッターセッター */

	/// @brief 選択されたアビリティのタイプを取得します。
	ABILITY_TYPE GetSelectedAbility()const{ return ConvertSelectionToAbility(_iSelectedAbility); }

	/// @brief 	// 選択されたアビリティ番号を取得します。
	int GetSelectedAbilityIndex()const { return _iSelectedAbility; }	

	/// @brief 現在選択中のアビリティのインデックスを取得します。
	bool GetIsSelectComplete()const{ return _bIsSelectComplete; }		

	/// @brief 選択画面がアクティブかどうかを取得します。
	bool GetIsScreenActive()const { return _bIsScreenActive; }			

	/// @brief 選択状態を取得します。
	SelectionState GetSelectionState()const{ return _selectionState; }

	/// @brief 選択状態を設定します。
	void SetSelectionState(SelectionState state){ _selectionState = state; }


	/* クラスセット */

	/// @brief プレイヤーマネージャーを設定します。
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager) { _playerManager = playerManager; }

	/// @brief プレイヤーアンロックマネージャーを設定します。
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

