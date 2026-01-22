#pragma once
#include "appframe.h"
#include "CharaBase.h"

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
	float invincibleDuration;   // 無敵持続時間
	float startTime;            // 開始時間
	float activeTime;           // アクティブ時間
	float recoveryTime;         // 硬直時間
	float dodgeMoveSpeed;       // 回避移動速度
};

class DodgeSystem
{
public:
	DodgeSystem();
	virtual ~DodgeSystem();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	// 回避設定初期化
	bool InitializeConfig
	(
		float invincibleDuration,   // 無敵持続時間
		float startTime,			// 開始時間
		float activeTime,			// アクティブ時間
		float recoveryTime,			// 硬直時間
		float dodgeMoveSpeed		// 回避移動速度
	);

	// 回避呼び出し
	void CallDodge(std::shared_ptr<CharaBase>chara, DodgeConfig& config);

	// 回避処理
	void StartDodge();										// 回避開始
	void StopDodge();										// 回避停止
	void UpdateDodgeState();								// 回避状態更新
	void UpdateDodgePos(std::shared_ptr<CharaBase>chara);	// 回避位置更新
	void UpdateInvincible();								// 無敵状態更新
	bool CanDodge() const;									// 回避可能かチェック
	bool IsDodging() const;									// 回避中かチェック

	// ゲッターセッター
	DODGE_STATE GetDodgeState() const { return _eDodgeState; }				// 回避状態取得
	void SetConfig(const DodgeConfig& config) { _stcDodgeConfig = config; }	// 回避設定設定

	bool GetIsInvincible() const { return _bIsInvincible; }	// 無敵状態かチェック

protected:
	DODGE_STATE _eDodgeState;		// 回避状態
	DodgeConfig _stcDodgeConfig;	// 回避設定

	// 回避関係
	float _currentTime;		// 経過時間
	float _invincibleTime;	// 無敵経過時間
	bool _bIsDodging;		// 回避中フラグ
	bool _bIsInvincible;	// 無敵状態フラグ
};

