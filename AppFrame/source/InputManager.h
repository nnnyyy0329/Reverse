#pragma once
#include "DxLib.h"

// ゲーム内のアクション定義
// キー割り当てを一元管理する
enum class INPUT_ACTION
{
	// 移動
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,

	// アクション
	ATTACK,
	DODGE,
	DASH,
	ABILITY,
	TRANSFORM,

	// 項目選択
	UP,
	DOWN,
	LEFT,
	RIGHT,

	// システム
	MENU,
	SELECT,

	// デバッグ用
	DEBUG1,
	DEBUG2,
	DEBUG3,

	NUM_ACTIONS// アクション数(最後に置く)
};

// アナログスティック状態
struct AnalogState
{
	float lx = 0.0f;// 左スティックX軸
	float ly = 0.0f;// 左スティックY軸
	float rx = 0.0f;// 右スティックX軸
	float ry = 0.0f;// 右スティックY軸
	float lz = 0.0f;
	float rz = 0.0f;
};

// ハットスイッチ状態
struct HatState
{
	int angle = -1;// ハットスイッチの角度、未入力は-1
	int oldAngle = -1;// 前フレームの角度

	bool IsUp() { return (angle >= 0 && angle < 2250) || (angle >= 33750); }
	bool IsRight() { return (angle >= 6750 && angle < 11250); }
	bool IsDown() { return (angle >= 17750 && angle < 20250); }
	bool IsLeft() { return (angle >= 24750 && angle < 29250); }
	bool IsNeutral() { return angle == -1; }

	bool WasUp() { return (oldAngle >= 0 && oldAngle < 2250) || (oldAngle >= 33750); }
	bool WasRight() { return (oldAngle >= 6750 && oldAngle < 11250); }
	bool WasDown() { return (oldAngle >= 17750 && oldAngle < 20250); }
	bool WasLeft() { return (oldAngle >= 24750 && oldAngle < 29250); }
};

// トリガー(LT,RT)状態
// DxLibのZ軸、Rz軸を -1000~+1000 で取得して正規化
struct TriggerButtonState
{
	float lt = 0.0f;// LT(0.0~1.0)
	float rt = 0.0f;// RT(0.0~1.0)
	bool ltHold = false;// LTホールド
	bool rtHold = false;// RTホールド
	bool ltTrg = false;// LTトリガー
	bool rtTrg = false;// RTトリガー
	bool ltRel = false;// LTリリース
	bool rtRel = false;// RTリリース
};

// 毎フレーム Update() を呼ぶことで入力状態を更新する
class InputManager
{
public:
	// シングルトンインスタンス取得
	static InputManager* GetInstance();

	static InputManager* GetNullInstance();

	void Update();

	void ResetInput();// 入力状態をリセットする

	// 入力状況
	bool IsHold(INPUT_ACTION action);// 押し続けている間 true
	bool IsTrigger(INPUT_ACTION action);// 押した瞬間だけ true
	bool IsRelease(INPUT_ACTION action);// 離した瞬間だけ true

	// アナログスティック状態
	const AnalogState& GetAnalog() { return _analog; }
	float GetAnalogMin() { return _analogMin; }
	void SetAnalogMin(float val) { _analogMin = val; }// アナログ閾値の変更

	// ハットスイッチ状態
	const HatState& GetHat() { return _hat; }

	// トリガー状態
	const TriggerButtonState& GetTrigger() { return _trigger; }
	void SetTriggerMin(float val) { _triggerMin = val; }// トリガー閾値の変更

private:
	InputManager() {};
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	// アクションに対応するパッドコードを取得する
	int GetPadCode(INPUT_ACTION action);

	// アクションに対応するキーコードを取得する
	int GetKeyCode(INPUT_ACTION action);

	// 内部状態
	// パッド
	int _padKey = 0;
	int _padTrg = 0;
	int _padRel = 0;

	// キーボード
	bool _bKeyHold[static_cast<int>(INPUT_ACTION::NUM_ACTIONS)] = {};
	bool _bKeyTrg[static_cast<int>(INPUT_ACTION::NUM_ACTIONS)] = {};
	bool _bKeyRel[static_cast<int>(INPUT_ACTION::NUM_ACTIONS)] = {};

	// アナログスティック
	AnalogState _analog;
	float _analogMin = 0.3f;// アナログ閾値

	// ハットスイッチ
	HatState _hat;

	// トリガー
	TriggerButtonState _trigger;
	float _triggerMin = 0.3f;// トリガー閾値
};

