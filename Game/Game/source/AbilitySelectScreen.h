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
class AbilityActionHint;

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

	/// @brief デバッグ描画
	void DebugRender();

	/* 選択関係関数 */
	
	/// @brief 入力に応じて選択画面の表示を切り替える処理
	void SelectScreenByInput();		
	
	/// @brief 入力に応じて選択処理
	void SelectionByInput();		
	
	/// @brief 選択要素の表示
	void SelectRender();			
	
	/// @brief 選択画面の表示
	void SelectFrameRender();		
	
	/// @brief 選択をリセット
	void ResetSelection();			
	
	/// @brief 選択を完了する処理
	void CompleteSelection();	
	
	/// @brief プレイヤータイプに応じて選択可能かどうかをチェック
	bool IsSelectActiveByPlayerType()const;	

	
	/* デバッグ表示関連 */

	/// @brief デバッグ用の選択されたアビリティの表示
	void DebugSelectedAbilityRender();


	/* ゲッターセッター */

	/// @brief 選択状態を取得
	SelectionState GetSelectionState()const{ return _selectionState; }

	/// @brief 選択されたアビリティのタイプを取得
	ABILITY_TYPE GetSelectedAbility()const{ return ConvertSelectionToAbility(_iSelectedAbility); }

	/// @brief 選択画面がアクティブかどうかを取得
	bool GetIsScreenActive()const{ return _bIsScreenActive; }			

	/// @brief 選択されたアビリティの番号を取得・設定
	int GetSelectedAbilityIndex()const{ return _iSelectedAbility; }
	void SetSelectedAbilityIndex(int index){ _iSelectedAbility = index; }

	/// @brief 選択状態を設定
	void SetSelectionState(SelectionState state){ _selectionState = state; }


	/* クラスセット */

	/// @brief プレイヤーマネージャーを設定
	void SetPlayerManager(std::shared_ptr<PlayerManager> playerManager) { _playerManager = playerManager; }

	/// @brief プレイヤーアンロックマネージャーを設定
	void SetPlayerUnlockManager(std::shared_ptr<PlayerUnlockManager> unlockManager) { _playerUnlockManager = unlockManager; }

	/// @brief アビリティアクションヒントを設定
	void SetAbilityActionHint(std::shared_ptr<AbilityActionHint> actionHint) { _abilityActionHint = actionHint; }

private:


protected:
	std::shared_ptr<PlayerManager> _playerManager;				// プレイヤーマネージャー
	std::shared_ptr<PlayerUnlockManager> _playerUnlockManager;	// プレイヤーアンロックマネージャー
	std::shared_ptr<AbilityActionHint> _abilityActionHint;		// アビリティアクションヒント

	// 表示用ハンドル
	int _iHandle1;
	int _iHandle2;
	int _iHandle3;

	// 選択状態の管理
	SelectionState _selectionState;	// 選択状態
	int _iCurrentSelection;			// 現在選択中のアビリティ
	int _iSelectedAbility;			// 選択されたアビリティ

	// 表示位置関連
	float _fDrawCenterX;		// 描画中心X座標
	float _fSecondDrawCenterX;	// 2つ目の描画中心X座標
	
	// 選択完了と画面表示のフラグ
	bool _bIsSelectComplete;	// 選択完了フラグ
	bool _bIsScreenActive;		// 選択画面表示フラグ

	// 選択カーソル用
	int _iCursorCount;
	bool _bShowCursor;
};

