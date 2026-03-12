#include "ModeMenu.h"
#include "ApplicationMain.h"
#include "ApplicationGlobal.h"
#include "CameraManager.h"

bool ModeMenu::Initialize() {
	if (!base::Initialize()) { return false; }

	_curPos = 0;
	_curAnimCnt = 0;
	_bUseDebugCamera = false;

	return true;
}

bool ModeMenu::Terminate() {
	base::Terminate();

	ClearMenuItems();
	//_debugCamera.reset();
	_cameraManager.reset();

	return true;
}

bool ModeMenu::Process() {
	base::Process();
	InputManager* input = InputManager::GetInstance();

	// このモードより下のレイヤーはProcess()を呼ばない
	ModeServer::GetInstance()->SkipProcessUnderLayer();
	
	if(_bUseDebugCamera)
	{
		// デバッグカメラがONならProcess()を呼ぶ
		if(_cameraManager->GetIsUseDebugCamera())
		{
			_cameraManager->Process();
		}
	}
	else
	{
		// ほかの処理
	}


	// startでメニューを閉じる
	bool close = false;
	if (input->IsTrigger(INPUT_ACTION::MENU))
	{
		_cameraManager->SetIsUseDebugCamera(false); // デバッグカメラOFFにする
		close = true;
	}

	// 上下でカーソル移動をする
	if (input->IsTrigger(INPUT_ACTION::UP)) { _curPos--; _curAnimCnt = 0; }
	if (input->IsTrigger(INPUT_ACTION::DOWN)) { _curPos++; _curAnimCnt = 0; }



	// カーソル位置を上下ループ
	int itemNum = _menuItems.size();
	_curPos = (_curPos + itemNum) % itemNum;

	// AでアイテムのSelected()を呼ぶ
	if (input->IsTrigger(INPUT_ACTION::SELECT))
	{
		int ret = _menuItems[_curPos]->Selected();

		if (ret == 1) 
		{
			 //メニューを閉じる
			close = true;
		}
	}



	// メニューを閉じる
	if (close) {
		// このモードを削除する
		ModeServer::GetInstance()->Del(this);
	}

	_curAnimCnt++;

	return true;
}

bool ModeMenu::Render() {
	base::Render();

	if(_bUseDebugCamera){ return false; }

	// メニュー項目の確認
	int x = 128, y = 128, w = 0, h = 0, fontSize = 32, fontPitch = fontSize + 8;
	//SetFontSize(fontSize);
	for (auto ite = _menuItems.begin(); ite != _menuItems.end(); ite++) {
		int itemW = GetDrawStringWidth((*ite)->_text.c_str(), strlen((*ite)->_text.c_str()));
		if (w < itemW) {
			w = itemW;
		}
		h += fontPitch;
	}
	// カーソル, 枠分のサイズ拡張
	w += 64 + 16; h += 16;

	// 下地の描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(x, y, x + w, y + h, GetColor(0, 0, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawBox(x, y, x + w, y + h, GetColor(0, 0, 255), TRUE);

	// メニュー項目の描画
	int startY = 16 / 2;
	h = 0;
	for(auto ite = _menuItems.begin(); ite != _menuItems.end(); ite++) {
		DrawString(x + 64, y + startY + h, (*ite)->_text.c_str(), GetColor(255, 0, 0));
		h += fontPitch;
	}

	DrawGraph(x + 4 + ((_curAnimCnt / 6) % 4) * 4, y + startY + fontPitch * _curPos, gGlobal._cgCursor, TRUE);

	// デバッグカメラ状態表示
	DrawString(x + 16, y + h + startY, _bUseDebugCamera ? "DebugCam:ON" : "DebugCam:OFF", GetColor(255, 255, 0));


	return true;
}

void ModeMenu::AddMenuItem(MenuItemBase* item) {
	_menuItems.push_back(std::unique_ptr<MenuItemBase>(item));
}

void ModeMenu::ClearMenuItems() {
	// 登録したアイテムをすべて削除する
	_menuItems.clear();
}