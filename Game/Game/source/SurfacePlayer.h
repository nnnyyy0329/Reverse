/*****************************************
* file	 SurfacePlayer.h
* brief  表プレイヤークラス
* author 成田 悠真
* date   2025/12/09
******************************************/

#pragma once
#include "PlayerBase.h"

// 表プレイヤー用定数
namespace SurfacePlayerConstants
{
	constexpr float DAMAGE_MULTIPLIER = 5.0f;	// 表プレイヤー専用のダメージ倍率
}

/// @brief 表プレイヤークラス
class SurfacePlayer : public PlayerBase
{
public:

	SurfacePlayer();
	virtual ~SurfacePlayer();


	/* 基本関数 */

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	/// @brief デバッグ描画
	virtual void DebugRender() override;

	/// @brief 被ダメージ処理
	/// 
	/// @param fDamage ダメージ量
	/// @param eType 攻撃の所有者タイプ
	/// @param attackInfo 攻撃コリジョン情報
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE ownerType, const AttackCollision& attackInfo) override;

	/// @brief 弾による被ダメージ処理
	///
	/// @param fDamage ダメージ量
	/// @param char 弾のキャラタイプ
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara) override;


	/* ゲッターセッター */


private:

	/* 仮想関数のオーバーライド */

	/// @brief プレイヤー設定を取得
	///
	/// @return プレイヤー設定構造体
	virtual PlayerConfig		GetPlayerConfig()		override;

	/// @brief プレイヤーアニメーション設定を取得
	///
	/// @return プレイヤーアニメーション設定構造体
	virtual PlayerAnimations	GetPlayerAnimation()	override;

	/// @brief 描画設定を取得
	///
	/// @return 描画設定構造体
	virtual RenderConfig		GetRenderConfig()		override;

	/// @brief 回避設定を取得
	///
	/// @return 回避設定構造体
	virtual DodgeConfig			GetDodgeConfig()		override;

	/// @brief シールド設定を取得
	///
	/// @return シールド設定構造体
	virtual ShieldConfig		GetShieldConfig()		override;

	/// @brief 吸収攻撃システムのconst版ゲッター
	///
	/// @return 吸収攻撃システムのconstポインタ
	const PlayerAbsorbAttackSystem* GetAbsorbAttackSystemConst() const;

	/// @brief 吸収攻撃システムの非const版ゲッター
	///
	/// @return 吸収攻撃システムのポインタ
	PlayerAbsorbAttackSystem* GetAbsorbAttackSystem() override;


	/* 吸収攻撃関連 */
	
	/// @brief 吸収攻撃設定を取得
	///
	/// @return 吸収攻撃設定構造体
	AbsorbConfig GetAbsorbConfig();

	/// @brief 吸収攻撃システムの生成
	void MakeAbsorbSystem();		
	
	/// @brief 吸収攻撃システムの初期化
	void InitializeAbsorbSystem();

	/// @brief 吸収攻撃システムの処理
	void ProcessAbsorbSystem();		
	
	/// @brief 吸収攻撃システムのデバッグ描画
	void AbsorbSystemDebugRender();	


	/* 吸収攻撃のモーション管理用メンバ関数 */

	/// @brief 吸収攻撃モーション切り替え条件の更新
	void UpdateChangeAbsorbMotion();

	/// @brief 吸収攻撃の構え状態に移行する処理
	void StartAbsorbReadyState();

	/// @brief 吸収構えモーションが終了したときの処理
	void ProcessAbsorbReadyCompleted();

	/// @brief 吸収終了状態の処理
	void ProcessAbsorbFinish();

	/// @brief 吸収攻撃の停止処理
	void StopAbsorb();

	/// @brief 吸収構えのキャンセル処理
	void CancelAbsorbReady();

	/// @brief 吸収終了時に通常モーションに戻す処理
	void ReturnNormalMotion();
	
	/// @brief 吸収アニメーションの再生時間をデバッグ表示する関数
	void DebugDrawAbsorbAnimationTime();

	/// @brief 吸収攻撃の入力チェック
	///
	/// @return 吸収攻撃の入力がされているならtrue、されていないならfalse
	bool IsAbsorbInput() const { return (InputManager::GetInstance().IsHold(INPUT_ACTION::ABILITY)) != 0; }

	/// @brief 吸収攻撃がアクティブかどうかをチェック
	///
	/// @return 吸収攻撃システムが存在し、かつ吸収攻撃がアクティブならtrue、そうでないならfalse
	bool IsAbsorbActive() const { return _absorbAttackSystem && _absorbAttackSystem->IsAbsorbActive(); }

	/// @brief 吸収終了状態中に吸収攻撃の入力がされているかどうかをチェック
	///
	/// @return 吸収攻撃の入力がされていて、かつ吸収終了状態で、アニメーションが終了していないならtrue、そうでないならfalse
	bool IsInputInAbsorbFinishState() const { return (IsAbsorbInput() && IsAbsorbEndState() && !IsAnimationFinishedConst()); }

	/// @brief 吸収状態が終了状態かどうかをチェック
	///
	/// @return 何かしらの吸収状態で、吸収状態が終了状態ならtrue、そうでないならfalse
	bool IsAbsorbEndState() const { return _playerState.IsStateAbsorbing() && _playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_END; }



	std::unique_ptr<PlayerAbsorbAttackSystem> _absorbAttackSystem;	// 吸収攻撃システム

protected:

	// 吸収攻撃のモーション管理用メンバ変数
	bool _bIsAbsorbReadyCompleted;	// 吸収構えアニメーション完了フラグ
	bool _bWasAbsorbKeyPressed;		// 前フレームで吸収攻撃キーが押されていたか

};

