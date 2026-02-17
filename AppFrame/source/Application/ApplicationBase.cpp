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

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	SetZBufferBitDepth(32);

	if (DxLib_Init() == -1)
	{	// エラーが起きたら直ちに終了
		return false;
	}
	SetDrawScreen(DX_SCREEN_BACK);		// 描画先画面を裏画面にセット

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
		return -1;
	}

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// 乱数初期化
	srand((unsigned int)time(NULL));

	// モードサーバの初期化
	_serverMode = new ModeServer();

	

	return true;
}

bool ApplicationBase::Terminate() 
{
	// Effekseerを終了する。
	Effkseer_End();

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

	//XINPUT_STATE di;
	//GetJoypadXInputState(DX_INPUT_PAD1, &di);
	//float lx,ly, rx, ry;
	//lx = (float)di.ThumbLX / 32768.f; ly = (float)di.ThumbLY / 32768.f;
	//rx = (float)di.ThumbRX / 32768.f; ry = (float)di.ThumbRY / 32768.f;
	//// Xboxコントローラの場合
	//_analog.lx = -lx;
	//_analog.ly = -ly;
	//_analog.rx = rx;
	//_analog.ry = ry;

	// アナログスティック対応
	DINPUT_JOYSTATE di;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
	float lx, ly, rx, ry,lz,rz;// 左右アナログスティックの座標
	// Logicoolパッドの場合
	lx = (float)di.X / 1000.f; ly = (float)di.Y / 1000.f;// 左スティック
	rx = (float)di.Rx / 1000.f; ry = (float)di.Ry / 1000.f;// 右スティック
	lz = (float)di.Z / 1000.f; rz = (float)di.Rz / 1000.f;// トリガー（未使用）
	_analog.lx = lx;
	_analog.ly = ly;
	_analog.lz = lz;
	_analog.rx = rx;
	_analog.ry = ry;
	_analog.rz = rz;

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


