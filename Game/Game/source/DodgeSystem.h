#pragma once
#include "appframe.h"

// 回避状態列挙型
enum class DODGE_STATE
{
	NONE,
	INACTIVE,       // 非アクティブ
	STARTUP,        // 開始
	ACTIVE,         // 実行中
	RECOVERY,       // 終了硬直
	_EOT_
};

// 回避設定構造体
struct DodgeConfig
{
	float dodgeDuration;      // 回避持続時間
	float dodgeMoveSpeed;     // 回避移動速度
	float invincibleDuration; // 無敵持続時間
};

class DodgeSystem
{
public:
	DodgeSystem();
	virtual ~DodgeSystem();

	bool InitializeConfig();
	bool Terminate();
	bool Process();
	bool Render();

	void StartDodge();			// 回避開始
	void StopDodge();			// 回避停止
	void UpdateDodgeState();	// 回避状態更新
	void UpdateDodgePos();		// 回避位置更新

	bool IsDodging() const { return _bIsDodging; }			// 回避中かチェック
	bool IsInvincible() const { return _bIsInvincible; }	// 無敵状態かチェック

	// ゲッターセッター
	DODGE_STATE GetDodgeState() const { return _eDodgeState; }				// 回避状態取得
	void SetConfig(const DodgeConfig& config) { _stcDodgeConfig = config; }	// 回避設定設定

protected:
	DODGE_STATE _eDodgeState;		// 回避状態
	DodgeConfig _stcDodgeConfig;	// 回避設定

	// 回避関係
	bool _bIsDodging;          // 回避中フラグ
	bool _bIsInvincible;       // 無敵状態フラグ
};

