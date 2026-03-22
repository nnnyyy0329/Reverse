#pragma once
#include "appframe.h"
#include "CharaBase.h"

/// @brief 回避キャラ
enum class DODGE_CHARA
{
	NONE,
	SURFACE_PLAYER,   ///< 表プレイヤー
	INTERIOR_PLAYER,  ///< 裏プレイヤー
	BULLET_PLAYER,    ///< 弾プレイヤー
	ENEMY,
	_EOT_
};

/// @brief 回避状態列挙型
enum class DODGE_STATE
{
	NONE,
	INACTIVE,       ///< 非アクティブ
	STARTUP,        ///< 開始
	ACTIVE,         ///< 実行中
	RECOVERY,       ///< 終了硬直
	_EOT_
};

/// @brief 回避設定構造体
struct DodgeConfig
{
	DODGE_CHARA charaType;		// 回避キャラタイプ
	float invincibleDuration;	// 無敵持続時間
	float startTime;			// 開始時間
	float activeTime;			// アクティブ時間
	float recoveryTime;			// 硬直時間
	float dodgeMoveSpeed;		// 回避移動速度
	std::string soundName;		// サウンド名
};

/// @brief 回避システムクラス
class DodgeSystem
{
public:
	DodgeSystem();
	virtual ~DodgeSystem();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	/// @brief デバッグ情報描画
	void DebugRender();	


	/* 回避の登録と呼び出し */

	/// @brief  回避設定の登録
	///
	/// @param charaType 登録するキャラタイプ
	/// @param config 登録する回避情報
	void RegisterDodgeCharaConfig(DODGE_CHARA charaType, const DodgeConfig& config);

	/// @brief  回避呼び出し
	///
	/// @param chara 呼び出すキャラのポインタ
	/// @param charaType 呼び出すキャラタイプ
	void CallDodge(std::shared_ptr<CharaBase>chara, DODGE_CHARA charaType);


	/* 回避処理 */

	/// @brief 回避開始
	void StartDodge();	

	/// @brief 回避停止		
	void StopDodge();	

	/// @brief 回避状態更新
	void UpdateDodgeState();								

	/// @brief 回避キャラの位置更新
	///
	/// @param chara 位置更新する回避キャラのポインタ
	void UpdateDodgePos(std::shared_ptr<CharaBase>chara);

	/// @brief 無敵状態更新
	void UpdateInvincible();

	/// @brief キャラクターのコリジョン位置更新
	///
	/// @param chara 更新するコリジョンの所有者キャラ
	/// @param pos 更新する基準の位置
	void UpdateDodgeCharaCol(std::shared_ptr<CharaBase>chara, const VECTOR& pos);

	/// @brief 回避可能かチェック
	///
	/// @return 回避を始められるなら true 、始められないなら false
	bool CanDodge() const;	

	/// @brief 回避中かチェック
	///
	/// @return 回避中なら true 、そうでないなら false
	bool IsDodging() const;	

	/// @brief 指定キャラが無敵状態かチェック
	/// 
	/// @param 指定キャラ
	/// 
	/// @return 無敵中なら true 、無敵中でないなら false
	bool IsCharacterInvincible(std::shared_ptr<CharaBase> chara) const;	

	/// @brief 無敵状態デバッグ描画
	void InvincibleDebugRender();


	/* ゲッターセッター */

	/// @brief 現在回避中のキャラ取得
	///
	/// @return 現在回避中のキャラ
	std::weak_ptr<CharaBase> GetCurrentDodgeChara() const { return _currentDodgeChara; }	

	/// @brief 回避キャラ取得
	///
	/// @return 回避キャラ
	DODGE_CHARA GetDodgeChara() const { return _eDodgeChara; }					

	/// @brief 回避状態取得
	///
	/// @return 回避状態
	DODGE_STATE GetDodgeState() const { return _eDodgeState; }					

	/// @brief 回避情報設定
	///
	/// @param 設定する回避情報
	void SetConfig(const DodgeConfig& config) { _stcDodgeConfig = config; }		

	/// @brief 無敵中か取得
	///
	/// @return 無敵中かのフラグ
	bool GetIsInvincible() const { return _bIsInvincible; }									

protected:
	// キャラタイプ別設定管理
	std::map<DODGE_CHARA, DodgeConfig>_charaConfigs;

	// 回避状態管理
	DODGE_CHARA _eDodgeChara;		// 回避キャラ
	DODGE_STATE _eDodgeState;		// 回避状態
	DodgeConfig _stcDodgeConfig;	// 回避設定

	// 回避関係
	float _currentTime;		// 経過時間
	float _invincibleTime;	// 無敵経過時間
	bool _bIsDodging;		// 回避中フラグ
	bool _bIsInvincible;	// 無敵状態フラグ

	// 現在回避しているキャラ
	std::weak_ptr<CharaBase> _currentDodgeChara;
};

