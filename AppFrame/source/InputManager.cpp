#include "InputManager.h"

// シングルトンインスタンスの実体
static InputManager* _nullInstance = nullptr;

InputManager* InputManager::GetNullInstance()
{
	if (_nullInstance == nullptr)
	{
		_nullInstance = new InputManager();
	}
	return _nullInstance;
}

InputManager* InputManager::GetInstance()
{
	if (_nullInstance == nullptr) {// インスタンスがなければ生成
		_nullInstance = new InputManager();
	}
	return _nullInstance;// インスタンスを返す
}

void InputManager::Update()
{
	// パッド入力
	int oldPad = _padKey;
	_padKey = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	_padTrg = (_padKey ^ oldPad) & _padKey;
	_padRel = (_padKey ^ oldPad) & ~_padKey;

	// アナログスティック
	DINPUT_JOYSTATE di;
	GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
	_analog.lx = static_cast<float>(di.X) / 1000.0f;
	_analog.ly = static_cast<float>(di.Y) / 1000.0f;
	_analog.rx = static_cast<float>(di.Rx) / 1000.0f;
	_analog.ry = static_cast<float>(di.Ry) / 1000.0f;
	_analog.lz = static_cast<float>(di.Z) / 1000.0f;
	_analog.rz = static_cast<float>(di.Rz) / 1000.0f;

	// キーボード入力
	const int numActions = static_cast<int>(INPUT_ACTION::NUM_ACTIONS);
	for (int i = 0; i < numActions; ++i)
	{
		INPUT_ACTION action = static_cast<INPUT_ACTION>(i);
		int keyCode = GetKeyCode(action);

		bool prevHold = _bKeyHold[i];
		bool curHold = (keyCode != -1) && (CheckHitKey(keyCode) != 0);

		_bKeyHold[i] = curHold;
		_bKeyTrg[i] = (!prevHold && curHold);
		_bKeyRel[i] = (prevHold && !curHold);
	}
}

// 入力状況
// パッドかキーボードどちらかが入力されていれば true を返す
bool InputManager::IsHold(INPUT_ACTION action)
{
	int padCode = GetPadCode(action);
	bool padHold = (padCode != 0) && ((_padKey & padCode) != 0);

	bool keyHold = _bKeyHold[static_cast<int>(action)];

	return padHold || keyHold;
}

bool InputManager::IsTrigger(INPUT_ACTION action)
{
	int padCode = GetPadCode(action);
	bool padTrg = (padCode != 0) && ((_padTrg & padCode) != 0);

	bool keyTrg = _bKeyTrg[static_cast<int>(action)];

	return padTrg || keyTrg;
}

bool InputManager::IsRelease(INPUT_ACTION action)
{
	int padCode = GetPadCode(action);
	bool padRel = (padCode != 0) && ((_padRel & padCode) != 0);

	bool keyRel = _bKeyRel[static_cast<int>(action)];

	return padRel || keyRel;
}

// パッドボタン　マッピング
int InputManager::GetPadCode(INPUT_ACTION action)
{
	switch (action)
	{
		// 移動(デジタル方向キー)
		case INPUT_ACTION::MOVE_UP:     return PAD_INPUT_UP;
		case INPUT_ACTION::MOVE_DOWN:   return PAD_INPUT_DOWN;
		case INPUT_ACTION::MOVE_LEFT:   return PAD_INPUT_LEFT;
		case INPUT_ACTION::MOVE_RIGHT:  return PAD_INPUT_RIGHT;
		// アクション
		case INPUT_ACTION::ATTACK:      return PAD_INPUT_6;// R1
		case INPUT_ACTION::DODGE:       return PAD_INPUT_4;// X
		case INPUT_ACTION::DASH:        return PAD_INPUT_9;// Lスティック押し込み
		case INPUT_ACTION::ABILITY:     return PAD_INPUT_6;// R1
		case INPUT_ACTION::TRANSFORM:   return PAD_INPUT_3;// Y
		// システム
		case INPUT_ACTION::MENU:        return PAD_INPUT_10;// START
		case INPUT_ACTION::SELECT:      return PAD_INPUT_1;// A

		default:						return 0;
	}
}

// キーボードキー　マッピング
int InputManager::GetKeyCode(INPUT_ACTION action)
{
	switch (action)
	{
		// 移動
		case INPUT_ACTION::MOVE_UP:     return KEY_INPUT_UP;
		case INPUT_ACTION::MOVE_DOWN:   return KEY_INPUT_DOWN;
		case INPUT_ACTION::MOVE_LEFT:   return KEY_INPUT_LEFT;
		case INPUT_ACTION::MOVE_RIGHT:  return KEY_INPUT_RIGHT;
		// アクション
		case INPUT_ACTION::ATTACK:      return KEY_INPUT_A;
		case INPUT_ACTION::DODGE:       return KEY_INPUT_D;
		case INPUT_ACTION::DASH:        return KEY_INPUT_LSHIFT;
		case INPUT_ACTION::ABILITY:     return KEY_INPUT_W;
		case INPUT_ACTION::TRANSFORM:   return KEY_INPUT_S;
		// システム
		case INPUT_ACTION::MENU:        return KEY_INPUT_SPACE;
		case INPUT_ACTION::SELECT:      return KEY_INPUT_Z;

		default:						return -1;
	}
}
