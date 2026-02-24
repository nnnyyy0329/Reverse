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

	// システム
	MENU,
	SELECT,

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

// 毎フレーム Update() を呼ぶことで入力状態を更新する
class InputManager
{
public:
	// シングルトンインスタンス取得
	static InputManager* GetInstance();

	static InputManager* GetNullInstance();

	void Update();

	// 入力状況
	bool IsHold(INPUT_ACTION action);// 押し続けている間 true
	bool IsTrigger(INPUT_ACTION action);// 押した瞬間だけ true
	bool IsRelease(INPUT_ACTION action);// 離した瞬間だけ true

	// アナログスティック状態
	const AnalogState& GetAnalog() { return _analog; }
	float GetAnalogMin() { return _analogMin; }
	void SetAnalogMin(float val) { _analogMin = val; }// アナログ閾値の変更

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
};

