#include "AbilitySelectScreen.h"

namespace
{
	const int MIN_SELECT = 0;
	const int MAX_SELECT = 2;
	const int BLINK_SPEED = 15;
}

AbilitySelectScreen::AbilitySelectScreen()
{
	_iHandle1 = ResourceServer::GetInstance()->GetHandle("select1");
	_iHandle2 = ResourceServer::GetInstance()->GetHandle("select2");
	_iHandle3 = ResourceServer::GetInstance()->GetHandle("select3");

	// 選択状態の初期化
	_iCurrentSelection = 1;		// 現在選択中のアビリティ
	_iSelectedAbility = -1;		// 決定されたアビリティ
	_bIsSelectComplete = false;	// 選択完了フラグ

	// カーソル選択用
	_iCursorCount = 0;		// 点滅用カウンター
	_bShowCursor = false;	// 点滅フラグ
}

AbilitySelectScreen::~AbilitySelectScreen()
{

}

bool AbilitySelectScreen::Initialize()
{
	return true;
}

bool AbilitySelectScreen::Terminate()
{
	return true;
}

bool AbilitySelectScreen::Process()
{
	// このモードより下のレイヤーはProcess()を呼ばない
	//ModeServer::GetInstance()->SkipProcessUnderLayer();

	// 入力による選択処理
	SelectionByInput();

	return true;
}

bool AbilitySelectScreen::Render()
{
	// 選択要素の表示
	SelectRender();

	// 選択画面表示
	SelectFrameRender();

	return true;
}

// 入力による選択処理
void AbilitySelectScreen::SelectionByInput()
{
	if(_bIsSelectComplete){ return; }

	// 左キーが押されたら
	if(_trg & PAD_INPUT_LEFT)
	{
		_iCurrentSelection--;	// 左に移動

		// 一番左からさらに左に移動したら
		if(_iCurrentSelection < MIN_SELECT)
		{
			// 一番右に移動
			_iCurrentSelection = MAX_SELECT;
		}
	}

	// 右キーが押されたら
	if(_trg & PAD_INPUT_RIGHT)
	{
		_iCurrentSelection++;	// 右に移動

		// 一番右からさらに右に移動したら
		if(_iCurrentSelection > MAX_SELECT)
		{
			// 一番左に移動
			_iCurrentSelection = MIN_SELECT;
		}
	}

	// Zキーで決定
	if(_trg & PAD_INPUT_1)
	{
		_iSelectedAbility = _iCurrentSelection;
		_bIsSelectComplete = true;				// 選択完了
	}

	// 点滅カウンターを進める
	_iCursorCount++;
}

// 選択されたアビリティに切り替え処理
void AbilitySelectScreen::SwitchAbilityBySelect()
{
	switch(_iSelectedAbility)
	{
		case 0:
		{
			break;
		}

		case 1:
		{
			break;
		}

		case 2:
		{
			break;
		}

		default:
		{
			break;
		}
	}
}

// 選択要素の表示
void AbilitySelectScreen::SelectRender()
{
	int x = 0;
	int y = 0;
	int w = 1920;
	int h = 1080;

	// 背景の半透明黒
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	//DrawBox(x, y, x + w, y + h, GetColor(255, 255, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// アビリティ画像の座標
	int selectX[3] = { 0, 640, 1280 };
	int selectY = 0;

	// アビリティ画像を描画
	DrawGraph(selectX[0], selectY, _iHandle1, TRUE);
	DrawGraph(selectX[1], selectY, _iHandle2, TRUE);
	DrawGraph(selectX[2], selectY, _iHandle3, TRUE);
}

// 選択画面表示
void AbilitySelectScreen::SelectFrameRender()
{
	// 選択し終わったか
	if(!_bIsSelectComplete)
	{
		_bShowCursor = (_iCursorCount / BLINK_SPEED) % 2 == 0;

		// カーソル表示
		if(_bShowCursor)
		{
			// アビリティ画像の座標
			int selectX[3] = { 0, 640, 1280 };
			int selectY = 0;

			// 選択中のアビリティの周りに枠を描画
			int frameX = selectX[_iCurrentSelection];
			int frameY = selectY;

			// 枠のサイズ（仮 640x1080）
			int frameW = 640;
			int frameH = 1080;

			// 選択フレームを描画
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(frameX - 5, frameY - 5, frameX + frameW + 5, frameY + frameH + 5, GetColor(255, 255, 0), TRUE);
			DrawBox(frameX - 3, frameY - 3, frameX + frameW + 3, frameY + frameH + 3, GetColor(0, 255, 0), TRUE);
			DrawBox(frameX - 1, frameY - 1, frameX + frameW + 1, frameY + frameH + 1, GetColor(255, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
	else
	{
		// 選択完了時の表示
		DrawString(100, 100, "アビリティが選択されました！", GetColor(0, 55, 0));
	}
}