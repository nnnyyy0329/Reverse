// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "PlayerAbsorbAttackSystem.h"

// 表プレイヤー
class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();	
	virtual bool Render();	

	// 共通関数のオーバーライド
	virtual void DebugRender()override;																		// デバッグ情報描画
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;	// 被ダメージ処理
	void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)override;										// 弾による被ダメージ処理

	// 純粋仮想関数のオーバーライド
	virtual PlayerConfig		GetPlayerConfig()						override;	// 設定を取得
	virtual PlayerAnimations	GetPlayerAnimation()					override;	// アニメーション設定を取得
	virtual RenderConfig		GetRenderConfig()						override;	// 描画設定を取得
	virtual DodgeConfig			GetDodgeConfig()						override;	// 回避設定を取得
	virtual ShieldConfig		GetShieldConfig()						override;	// シールド設定を取得
	virtual AttackConstants		GetAttackConstants()const				override;	// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[3])				override;	// 攻撃設定を取得
	virtual void GetDirAdjustConfigs(AttackDirAdjustConfig configs[3])	override;	// 攻撃方向補正設定を取得
	virtual AreaAttackConfig GetAreaAttackConfig()						override;	// 範囲攻撃設定を取得
	const PlayerAbsorbAttackSystem* GetAbsorbAttackSystem()const;	// 吸収攻撃システム取得

	/*****ゲッターセッター*****/

private:
	// 吸収攻撃システム
	std::unique_ptr<PlayerAbsorbAttackSystem> _absorbAttackSystem;

	// 吸収攻撃関連
	AbsorbConfig GetAbsorbConfig();	// 吸収攻撃設定取得
	void MakeAbsorbSystem();		// 吸収攻撃システム生成
	void InitializeAbsorbSystem();	// 吸収攻撃システム初期化
	void ProcessAbsorbSystem();		// 吸収攻撃システム処理
	void AbsorbSystemDebugRender();	// 吸収攻撃システムデバッグ描画

	/* 吸収攻撃のモーション管理用メンバ関数 */
	
	// 吸収攻撃モーション切り替え条件処理
	void ProcessChangeAbsorbMotion();

	// 構え状態に移行
	void StartAbsorbReadyState();

	// 構えモーション終了時の処理
	void ProcessAbsorbReadyCompleted();

	// 吸収終了時の処理
	void ProcessAbsorbFinish();

	// 吸収停止処理
	void StopAbsorb();

	// 吸収構えキャンセル処理
	void CancelAbsorbReady();

	// 吸収終了時に通常モーションに戻す処理
	void ReturnNormalMotion();

	// 吸収攻撃の入力チェック
	bool IsAbsorbInput()const;

	// 吸収アニメーション再生時間デバッグ表示
	void DebugDrawAbsorbAnimationTime();

	/* 吸収攻撃のモーション管理用メンバ変数 */

	bool _bIsAbsorbReadyCompleted = false;	// 吸収構えアニメーション完了フラグ
	bool _bWasAbsorbKeyPressed = false;		// 前フレームで吸収攻撃キーが押されていたか

protected:

};

