#include "AbilitySelectScreen.h"
#include "PlayerManager.h"
#include "PlayerUnlockManager.h"

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
	_selectionState = SelectionState::NOT_SELECTION;	// 選択状態
	_iCurrentSelection = 0;								// 現在選択中のアビリティ
	_iSelectedAbility = -1;								// 決定されたアビリティ

	_bIsSelectComplete = false;				// 選択完了フラグ
	_bIsScreenActive = false;				// 選択画面表示フラグ

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
	// 入力による選択処理
	SelectionByInput();

	// 入力による画面表示
	SelectScreenByInput();

	return true;
}

bool AbilitySelectScreen::Render()
{
	// 選択要素の表示
	SelectRender();

	// 選択中じゃない場合は選択画面表示しない
	if(_selectionState != SelectionState::SELECTING){ return false; }

	// 選択画面表示
	SelectFrameRender();

	return true;
}

// 入力による画面表示
void AbilitySelectScreen::SelectScreenByInput()
{

}

// 入力による選択処理
void AbilitySelectScreen::SelectionByInput()
{
	// 選択処理完了中は入力を受け付けない
	if(_selectionState == SelectionState::SELECT_COMPLETED ||
		_selectionState == SelectionState::COMPLETED)
	{
		return;
	}

	auto& im = InputManager::GetInstance();

	//// デバッグ用の入力
	//if(im.IsTrigger(INPUT_ACTION::DEBUG2))
	//{
	//	if(_playerUnlockManager)
	//	{
	//		// デバッグ用の強制解放
	//		_playerUnlockManager->ForceUnlock(ABILITY_TYPE::INTERIOR_PLAYER);	// 裏プレイヤー解放
	//		_playerUnlockManager->ForceUnlock(ABILITY_TYPE::BULLET_PLAYER);		// 弾プレイヤー解放
	//	}
	//}

	// エネルギーがたまったら能力を開放
	if(EnergyManager::GetInstance()->CanSwitchPlayer())
	{
		if(_playerUnlockManager)
		{
			// デバッグ用の強制解放
			_playerUnlockManager->ForceUnlock(ABILITY_TYPE::INTERIOR_PLAYER);	// 裏プレイヤー解放
			_playerUnlockManager->ForceUnlock(ABILITY_TYPE::BULLET_PLAYER);		// 弾プレイヤー解放
		}
	}

	// パワーアビリティ選択
	if(im.IsTrigger(INPUT_ACTION::SELECT_POWER))
	{
		// 裏プレイヤーが解放されているかチェック
		if(_playerUnlockManager && !_playerUnlockManager->IsAbilityUnlocked(ABILITY_TYPE::INTERIOR_PLAYER))
		{
			return;	// 解放されていない場合は何もしない
		}

		// 裏プレイヤー（配列インデックスの2番）
		_iSelectedAbility = 2;	

		// 選択確定状態に移行
		_selectionState = SelectionState::SELECT_COMPLETED;	

		// 入力をリセット
		im.ResetInput();
	}

	// 弾発射アビリティ選択
	if(im.IsTrigger(INPUT_ACTION::SELECT_BULLET))
	{
		// 弾プレイヤーが解放されているかチェック
		if(_playerUnlockManager && !_playerUnlockManager->IsAbilityUnlocked(ABILITY_TYPE::BULLET_PLAYER))
		{
			return;	// 解放されていない場合は何もしない
		}

		// 弾プレイヤー（配列インデックスの1番）
		_iSelectedAbility = 1;	

		// 選択確定状態に移行
		_selectionState = SelectionState::SELECT_COMPLETED;	

		// 入力をリセット
		im.ResetInput();
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
	// 選択中のみカーソル表示
	if(_selectionState == SelectionState::SELECTING)
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
	// 選択確定時は枠を常時表示
	else if(_selectionState == SelectionState::SELECT_COMPLETED)
	{
		// 画像のサイズを取得
		int graphW, graphH;
		GetGraphSize(_iHandle1, &graphW, &graphH);

		int selectX[3] = // 表示するX座標
		{
			Render::SELECT_DRAW_X,											// 表プレイヤー
			Render::SELECT_DRAW_X + graphW + Render::DRAW_OFFSET_X,			// 弾プレイヤー
			Render::SELECT_DRAW_X + graphW * 2 + Render::DRAW_OFFSET_X * 2	// 裏プレイヤー
		};
		int selectY = Render::SELECT_DRAW_Y; // 表示するY座標

		// 選択されたアビリティの周りに確定枠を描画
		int frameX = selectX[_iSelectedAbility];
		int frameY = selectY;

		// 枠のサイズ
		int frameW = 128;
		int frameH = 128;

		// 確定フレームを描画（点滅なし、異なる色）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
		DrawBox(frameX - 5, frameY - 5, frameX + frameW + 5, frameY + frameH + 5, GetColor(0, 255, 255), TRUE);
		DrawBox(frameX - 3, frameY - 3, frameX + frameW + 3, frameY + frameH + 3, GetColor(0, 200, 255), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

// 選択をリセットする関数
void AbilitySelectScreen::ResetSelection()
{
	_selectionState = SelectionState::NOT_SELECTION;	// 選択状態を非選択にリセット
	_iSelectedAbility = -1;								// 選択されたアビリティをリセット

	_bIsSelectComplete = false;
	_bIsScreenActive = false;
}

// 選択を完了する関数
void AbilitySelectScreen::CompleteSelection()
{
	_selectionState = SelectionState::COMPLETED;	// 選択状態を選択処理完了に設定
	_iCurrentSelection = 0;							// 現在選択中のアビリティをリセット
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