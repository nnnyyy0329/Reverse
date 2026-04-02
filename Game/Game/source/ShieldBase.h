/*****************************************
* file   ShieldBase.h
* brief  シールド基底クラス
* author 成田 悠真
* date   2026/02/05
******************************************/

#pragma once
#include "appframe.h"
#include "GeometryUtility.h"

// 前方宣言
class CharaBase;

/// @brief シールド発生キャラタイプ列挙型
enum class SHIELD_CHARA
{
	NONE,
	SURFACE_PLAYER,		///< 表プレイヤー
	INTERIOR_PLAYER,	///< 裏プレイヤー
	BULLET_PLAYER,		///< 弾プレイヤー
	ENEMY,				///< 敵キャラ
	_EOT_
};

/// @brief シールド状態列挙型
enum class SHIELD_STATE
{
	INACTIVE,	///< 非アクティブ
	STARTUP,	///< 発生前
	ACTIVE,		///< 攻撃判定中
	RECOVERY,	///< 硬直中
	_EOT_,
};

/// @brief シールド設定構造体
struct ShieldConfig
{
	SHIELD_CHARA charaType;			// シールド発生キャラタイプ
	float dotThreshold;				// シールド有効内積閾値
	float blockAngleRange;			// ブロック可能角度範囲
	float blockDistance;			// ブロック可能距離
	float startTime;				// 発生前時間
	float recoveryTime;				// 硬直時間
	float staminaCost;              // スタミナコスト
	std::string blockEffectName;	// ブロックエフェクト名
	float effectOffset;				// エフェクトオフセット
};

/// @brief シールド基底クラス
class ShieldBase
{
public:

	ShieldBase();
	virtual ~ShieldBase();


	/* 基本関数 */

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了	
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	void DebugRender();			// デバッグ描画


	/* シールド操作 */

	/// @brief シールド発動関数
	virtual void ActivateShield();		

	/// @brief シールド解除関数
	virtual void DeactivateShield();	


	/* ブロック判定 */
	
	/// @brief 攻撃をブロック可能かチェックする関数
	///
	/// @param attackPos 攻撃位置
	/// @param attackDir 攻撃方向
	/// 
	/// @return ブロック可能ならtrue、そうでないならfalse
	bool CanBlockAttack(const VECTOR& attackPos, const VECTOR& attackDir)const;

	/// @brief 対象がシールド有効範囲内かチェックする関数
	///
	/// @param targetPos 対象位置
	/// 
	/// @return シールド有効範囲内ならtrue、そうでないならfalse
	bool IsInShieldRange(const VECTOR& targetPos)const;							
	
	/// @brief 攻撃が有効な方向からかチェックする関数
	///
	/// @param attackDir 攻撃方向
	/// 
	/// @return 攻撃が有効な方向からならtrue、そうでないならfalse
	bool IsAttackFromValidDirection(const VECTOR& attackDir) const;				

	/// @brief シールドがアクティブかチェックする関数
	///
	/// @return シールドがアクティブならtrue、そうでないならfalse
	bool IsActive()const { return _eShieldState == SHIELD_STATE::ACTIVE; }		

	/// @brief ブロック成功処理関数
	///
	/// @param attackDir 攻撃方向
	void SuccessBlock(const VECTOR& attackDir);									


	/* シールド内部の処理 */

	/// @brief シールド状態更新関数
	void UpdateShieldState();	

	/// @brief 所有者位置取得関数
	/// 
	/// @return 所有者位置
	VECTOR GetOwnerPos()const;	

	/// @brief 所有者方向取得関数
	///
	/// @return 所有者方向
	VECTOR GetOwnerDir()const;	


	/* ガード更新処理 */

	/// @brief ガード入力更新関数
	///
	/// @param input 入力マネージャー
	void UpdateGuardInput(InputManager* input);		

	/// @brief ガード開始
	void StartGuard();
	
	/// @brief ガード停止
	void StopGuard();								

	/// @brief シールド設定取得関数
	/// 
	/// @param config シールド設定
	///
	/// @return シールド設定
	virtual void SetShieldConfig(const ShieldConfig& config) { _stcShieldConfig = config; }


	/* ガード状態チェック */

	/// @brief スタミナが足りているかチェックする関数
	///
	/// @return スタミナが足りているならtrue、そうでないならfalse
	bool HasStamina() const;	

	/// @brief ガード中かチェックする関数
	///
	/// @return ガード中ならtrue、そうでないならfalse
	bool IsGuarding() const { return _eShieldState == SHIELD_STATE::ACTIVE; }


	/* デバック関数 */

	/// @brief シールド有効範囲デバッグ描画関数
	void DrawShieldRangeDebug();	


	/* ゲッターセッター */

	/// @brief 所有者取得関数
	///
	/// @return 所有者のCharaBaseへの共有ポインタ
	std::shared_ptr<CharaBase> GetOwner() const { return _owner.lock(); }	

	/// @brief 所有者設定関数
	///
	/// @param owner 設定する所有者のCharaBaseへの共有ポインタ
	void SetOwner(std::shared_ptr<CharaBase> owner) { _owner = owner; }		

protected:

	// シールド状態
	SHIELD_CHARA _eShieldChara;			// シールド発生キャラ
	SHIELD_STATE _eShieldState;			// シールド状態
	std::weak_ptr<CharaBase> _owner;	// 所有者

	// シールド設定
	ShieldConfig _stcShieldConfig;	// シールド設定
	float _fStartupTimer;			// 発生前タイマー
	float _fRecoveryTimer;			// 硬直タイマー

	bool _bIsBlocking;				// ブロック中フラグ
};

