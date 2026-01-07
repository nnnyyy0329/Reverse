// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "AttackBase.h"

class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	// SurfacePlayer_Move.cppで定義
	void ProcessCall();				// Process呼び出し用関数
	void ProcessMovePlayer();		// プレイヤー移動処理
	void ProcessStatusAnimation();	// ステータスに応じたアニメーション処理
	void ProcessPlayAnimation();	// アニメーション再生処理
	void ProcessStanding();			// 着地処理
	void ProcessJump();				// ジャンプ処理
	void ProcessCrouch();			// しゃがみ処理
	void ProcessDeath();			// 死亡処理
	void ProcessDebug();			// デバッグ用関数

	// SurfacePlayer_Draw.cppで定義
	void DrawModel();				// モデル描画
	void DrawDebug();				// デバッグ用表示
	void DrawCoordinate();			// 座標の表示
	void DrawCapsuleCollision();	// カプセルコリジョン表示
	void DrawStatus();				// ステータス表示
	void DrawParameter();			// パラメーター表示
	void DrawAnimationName();		// 再生されているアニメーション名表示
	void DrawColPos();				// コリジョンの情報表示
	void DrawAttackColData();		// 受け取ったコリジョンのデータ表示

	// SurfacePlayer_Attack.cppで定義
	void ProcessAttackCall();		// 攻撃Process呼び出し用関数
	void ProcessAttackColPos();		// コリジョン位置の更新関数
	void ProcessAttack();			// 攻撃処理
	void ProcessBranchAttack();		// 攻撃分岐処理
	void ProcessFirstAttack();		// 第1攻撃処理
	void ProcessSecondAttack();		// 第2攻撃処理
	void ProcessThirdAttack();		// 第3攻撃処理
	void InitializeAttackData();	// 攻撃データ初期化
	void ReceiveAttackColData();	// 攻撃コリジョンの情報受け取り関数

	bool CanNextAttack();			// 次の攻撃が可能かチェック
	bool IsAttacking();				// 攻撃中かチェック

	/*****ゲッターセッター*****/
	// 攻撃コリジョン情報の受け取り用
	VECTOR GetAttackColTop(){ return _vAttackColTop; }
	VECTOR GetAttackColBottom(){ return _vAttackColBottom; }
	float GetAttackColR(){ return _fAttackColR; }

protected:

	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// デバッグ用
	bool	_bViewCollision;

	// 固有変数
	float _fVelY;			// Y方向の速度
	bool _bIsJumping;		// ジャンプ中かどうか
	bool _bIsStanding;		// 着地しているかどうか
	bool _bIsCrouching;		// しゃがんでいるかどうか
	bool _bIsStartCrouch;	// しゃがみ開始フラグ

	// 表示用オフセット
	int _iDrawSizeOffset;
	int _iDrawOffsetX;
	int _iDrawOffsetY;

	// 攻撃システム
	AttackBase _firstAttack;		// 第1攻撃
	AttackBase _secondAttack;		// 第2攻撃
	AttackBase _thirdAttack;		// 第3攻撃
	bool _bCanCombo;				// コンボ可能フラグ
	int _iComboCount;				// コンボカウント

	// 攻撃コリジョン情報の受け取り用
	VECTOR _vAttackColTop;
	VECTOR _vAttackColBottom;
	float _fAttackColR;
};

