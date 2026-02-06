#pragma once
#include "appframe.h"
#include "GeometryUtility.h"

// シールド発生キャラ
enum class SHIELD_CHARA
{
	NONE,
	SURFACE_PLAYER,   // 表プレイヤー
	INTERIOR_PLAYER,  // 裏プレイヤー
	BULLET_PLAYER,    // 弾プレイヤー
	ENEMY,
	_EOT_
};

// シールドの状態管理
enum class SHIELD_STATE
{
	INACTIVE,	// 非アクティブ
	STARTUP,	// 発生前
	ACTIVE,		// 攻撃判定中
	RECOVERY,	// 硬直中
	_EOT_,
};

// シールド設定構造体
struct ShieldConfig
{
	SHIELD_CHARA charaType;			// シールド発生キャラタイプ
	float dotThreshold;				// シールド有効内積閾値
	float blockAngleRange;			// ブロック可能角度範囲
	float blockDistance;			// ブロック可能距離
	float guardStartupTime;         // ガード開始時間
	float guardEndTime;             // ガード終了時間
	float staminaCost;              // スタミナコスト（毎フレーム）
	std::string blockEffectName;	// ブロックエフェクト名
	float effectOffset;				// エフェクトオフセット
};

// 前方宣言
class CharaBase;  

// シールド基底クラス
class ShieldBase
{
public:
	ShieldBase();
	virtual ~ShieldBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了	
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	void DebugRender();			// デバッグ描画

	// シールド操作
	virtual bool ActivateShield();			// シールド発動
	virtual void DeactivateShield();		// シールド解除
	virtual bool CanActivateShield()const;	// シールド発動可能かチェック

	// ブロック判定
	bool CanBlockAttack(const VECTOR& attackPos, const VECTOR& attackDir)const;	// 攻撃をブロック可能かチェック
	bool IsInShieldRange(const VECTOR& targetPos)const;							// 対象がシールド有効範囲内かチェック
	bool IsAttackFromValidDirection(const VECTOR& attackDir) const;				// 攻撃が有効な方向からかチェック
	bool IsActive()const { return _eShieldState == SHIELD_STATE::ACTIVE; }		// シールドがアクティブかチェック
	void SuccessBlock(const VECTOR& attackDir);									// ブロック成功処理

	// シールド内部の処理
	void UpdateTimers();		// タイマー更新
	void UpdateShieldState();	// シールド状態更新
	VECTOR GetOwnerPos()const;	// 所有者位置取得
	VECTOR GetOwnerDir()const;	// 所有者前方取得

	// ガード入力処理
	void StartGuardByInput();	// ガード入力開始
	void  StartGuard();			// ガード開始
	void  StopGuard();			// ガード停止
	void UpdateGuardDuration();	// ガード継続時間更新

	// ガード状態チェック
	bool IsGuarding() const { return _eShieldState == SHIELD_STATE::ACTIVE; }
	bool CanStartGuard() const;	// ガード開始可能かチェック
	bool HasStamina() const;	// スタミナが足りているかチェック

	// ゲッターセッター
	SHIELD_CHARA GetShieldChara() const { return _eShieldChara; }		// シールド発生キャラ取得
	void SetShieldChara(SHIELD_CHARA chara) { _eShieldChara = chara; }	// シールド発生キャラ設定

	SHIELD_STATE GetShieldState() const { return _eShieldState; }		// シールド状態取得
	void SetShieldState(SHIELD_STATE state) { _eShieldState = state; }	// シールド状態設定

	std::shared_ptr<CharaBase> GetOwner() const { return _owner.lock(); }	// 所有者取得
	void SetOwner(std::shared_ptr<CharaBase> owner) { _owner = owner; }		// 所有者設定

	// 設定取得（継承先で実装）
	virtual ShieldConfig GetShieldConfig() = 0;

protected:
	// シールド状態
	SHIELD_CHARA _eShieldChara;			// シールド発生キャラ
	SHIELD_STATE _eShieldState;			// シールド状態
	std::weak_ptr<CharaBase> _owner;	// 所有者

	// シールド設定
	ShieldConfig _config;	// シールド設定
	float _fStartupTimer;	// 発生タイマー
	float _fActiveTimer;	// アクティブタイマー
	float _fRecoveryTimer;	// 硬直タイマー

	bool _bIsGuardPressed;          // ガードボタンが押されているかフラグ
	bool _bWasGuardPressed;         // 前フレームガードボタンが押されていたかフラグ

	bool _bIsBlocking;		// ブロック中フラグ
};

