#include "AbilitySelectScreen.h"
#include "PlayerManager.h"

namespace Render
{
	constexpr int SELECT_DRAW_X = 1500;
	constexpr int SELECT_DRAW_Y = 950;
	constexpr int DRAW_OFFSET_X = 10;
	constexpr int DRAW_OFFSET_Y = 0;
}

namespace
{
	constexpr int MIN_SELECT = 0;	// 最小選択肢
	constexpr int MAX_SELECT = 2;	// 最大選択肢
	constexpr int BLINK_SPEED = 15;	// 点滅速度
}

AbilitySelectScreen::AbilitySelectScreen()
{
	_playerManager = nullptr;	// プレイヤーマネージャー

	_iHandle1 = ResourceServer::GetInstance()->GetHandle("select1");
	_iHandle2 = ResourceServer::GetInstance()->GetHandle("select2");
	_iHandle3 = ResourceServer::GetInstance()->GetHandle("select3");

	// 選択状態の初期化
	_iCurrentSelection = 0;		// 現在選択中のアビリティ
	_iSelectedAbility = -1;		// 決定されたアビリティ
	_bIsSelectComplete = false;	// 選択完了フラグ
	_bIsScreenActive = false;	// 選択画面表示フラグ

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
	// 入力による画面表示
	SelectScreenByInput();

	// 入力による選択処理
	SelectionByInput();

	return true;
}

bool AbilitySelectScreen::Render()
{
	if(!_bIsScreenActive){ return false; }

	// 選択要素の表示
	SelectRender();

	// 選択画面表示
	SelectFrameRender();

	return true;
}

// 入力による画面表示
void AbilitySelectScreen::SelectScreenByInput()
{
	//if(!IsSelectActiveByPlayerType()){ return; }	// プレイヤータイプに応じて選択可能かどうか

	if(_inputManager->IsTrigger(INPUT_ACTION::TRANSFORM))
	{
		_bIsScreenActive = !_bIsScreenActive;
	}
}

// 入力による選択処理
void AbilitySelectScreen::SelectionByInput()
{
	if(_bIsSelectComplete){ return; }	// 選択完了していたら処理しない
	if(!_bIsScreenActive){ return; }	// 選択画面が表示されていなかったら処理しない

	// 左キーが押されたら
	if(_inputManager->IsTrigger(INPUT_ACTION::MOVE_LEFT))
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
	if(_inputManager->IsTrigger(INPUT_ACTION::MOVE_RIGHT))
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
	if(_inputManager->IsTrigger(INPUT_ACTION::TRANSFORM))
	{
		_iSelectedAbility = _iCurrentSelection;	// 選択されたアビリティを保存
		_bIsSelectComplete = true;				// 選択完了
		_bIsScreenActive = false;				// 選択画面非表示
	}

	// 点滅カウンターを進める
	_iCursorCount++;
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

	// 画像のサイズを取得
	int graphW, graphH;
	GetGraphSize(_iHandle1, &graphW, &graphH);

	int selectX[3] = // 表示するX座標
	{ 
		Render::SELECT_DRAW_X,
		Render::SELECT_DRAW_X + graphW + Render::DRAW_OFFSET_X,
		Render::SELECT_DRAW_X + graphW * 2 + Render::DRAW_OFFSET_X * 2
	};
	int selectY = Render::SELECT_DRAW_Y; // 表示するY座標

	// アビリティ画像を描画
	DrawGraph(selectX[0], selectY, _iHandle1, TRUE);
	DrawGraph(selectX[1], selectY, _iHandle3, TRUE);
	DrawGraph(selectX[2], selectY, _iHandle2, TRUE);
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
			// 画像のサイズを取得
			int graphW, graphH;
			GetGraphSize(_iHandle1, &graphW, &graphH);

			int selectX[3] = // 表示するX座標
			{
				Render::SELECT_DRAW_X,
				Render::SELECT_DRAW_X + graphW + Render::DRAW_OFFSET_X,
				Render::SELECT_DRAW_X + graphW * 2 + Render::DRAW_OFFSET_X * 2
			};			
			int selectY = Render::SELECT_DRAW_Y;

			// 選択中のアビリティの周りに枠を描画
			int frameX = selectX[_iCurrentSelection];
			int frameY = selectY;

			// 枠のサイズ（仮 640x1080）
			int frameW = 128;
			int frameH = 128;

			// 選択フレームを描画
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
			DrawBox(frameX - 5, frameY - 5, frameX + frameW + 5, frameY + frameH + 5, GetColor(255, 255, 0), TRUE);
			DrawBox(frameX - 3, frameY - 3, frameX + frameW + 3, frameY + frameH + 3, GetColor(0, 255, 0), TRUE);
			DrawBox(frameX - 1, frameY - 1, frameX + frameW + 1, frameY + frameH + 1, GetColor(255, 0, 0), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}

// 選択をリセットする関数
void AbilitySelectScreen::ResetSelection()
{
	_bIsSelectComplete = false;
	_iSelectedAbility = -1;
}

// プレイヤータイプに応じて選択可能かどうか
bool AbilitySelectScreen::IsSelectActiveByPlayerType()const
{
	if(!_playerManager){ return false; }
	
	// アクティブプレイヤーのタイプを取得
	PLAYER_TYPE activeType = _playerManager->GetActivePlayerType();

	// 表プレイヤー以外は選択不可
	if(activeType != PLAYER_TYPE::SURFACE)
	{
		return false;	// 表プレイヤー以外は選択不可
	}

	return true;	// 表プレイヤーなら選択可能
}