#pragma once
#include "appframe.h"
#include "AbilityBase.h"  

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
	void SelectScreenByInput();		// 入力による画面表示
	void SelectionByInput();		// 入力による選択処理
	void SwitchAbilityBySelect();	// 選択されたアビリティに切り替え処理
	void SelectRender();			// 選択要素の表示
	void SelectFrameRender();		// 選択画面表示
	void ResetSelection();			// 選択をリセットする関数を追加

	// ゲッターセッター
	ABILITY_TYPE GetSelectedAbility()const	// 選択されたアビリティ取得
	{
		return ConvertSelectionToAbility(_iSelectedAbility);
	}
	int GetSelectedAbilityIndex()const { return _iSelectedAbility; }	// 選択されたアビリティ番号取得
	bool GetIsSelectComplete()const{ return _bIsSelectComplete; }		// 選択完了フラグ取得

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}

protected:
	// 表示用ハンドル
	int _iHandle1;
	int _iHandle2;
	int _iHandle3;

	// 選択状態の管理
	int _iCurrentSelection;		// 現在選択中のアビリティ
	int _iSelectedAbility;		// 決定されたアビリティ
	bool _bIsSelectComplete;	// 選択完了フラグ
	bool _bIsScreenActive;		// 選択画面表示フラグ

	// 選択カーソル用
	int _iCursorCount;
	bool _bShowCursor;

	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;
};

