
#include "ApplicationBase.h"

ApplicationBase	*ApplicationBase::_lpInstance = NULL;


ApplicationBase::ApplicationBase()
{
	_lpInstance = this;
}

ApplicationBase::~ApplicationBase() 
{

}

bool ApplicationBase::Initialize(HINSTANCE hInstance)
{

	// DXライブラリの初期化
	if(AppWindowed())
	{
		ChangeWindowMode(true);							// ウィンドウモードに指定する
	}
	SetGraphMode(DispSizeW(), DispSizeH(), 32);

	if (DxLib_Init() == -1)
	{	// エラーが起きたら直ちに終了
		return false;
	}
	SetDrawScreen(DX_SCREEN_BACK);		// 描画先画面を裏画面にセット

	// 乱数初期化
	srand((unsigned int)time(NULL));

	// モードサーバの初期化
	_serverMode = new ModeServer();

	return true;
}

bool ApplicationBase::Terminate() 
{
	// DXライブラリ開放
	DxLib_End();

	return true;
}


bool ApplicationBase::Input()
{
	// キーの入力、トリガ入力を得る
	int keyold = _gKey;
	_gKey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	_gTrg = (_gKey ^ keyold) & _gKey;	// キーのトリガ情報生成（押した瞬間しか反応しないキー情報）

	// アナログスティック対応
	DINPUT_JOYSTATE di;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
	float lx, ly, rx, ry;// 左右アナログスティックの座標
	// Logicoolパッドの場合
	lx = (float)di.X / 1000.f; ly = (float)di.Y / 1000.f;// 左スティック
	rx = (float)di.Z / 1000.f; ry = (float)di.Rz / 1000.f;// 右スティック
	_analog.lx = lx;
	_analog.ly = ly;
	_analog.rx = rx;
	_analog.ry = ry;

	return true;
}

bool ApplicationBase::Process()
{
	_serverMode->ProcessInit();
	_serverMode->Process();
	_serverMode->ProcessFinish();
	return true;
}

bool ApplicationBase::Render() 
{
	_serverMode->RenderInit();
	_serverMode->Render();
	_serverMode->RenderFinish();
	return true;
}
