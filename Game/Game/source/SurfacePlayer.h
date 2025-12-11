/*ーーーーーーーーーー
|
|	成田担当
|
ーーーーーーーーーー*/

#pragma once
#include "PlayerBase.h"

class SurfacePlayer : public PlayerBase
{
public:
	SurfacePlayer();
	virtual ~SurfacePlayer();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	void MovePlayer();				// プレイヤー移動処理
	void StatusAnimationProcess();	// ステータスに応じたアニメーション処理
	void StandingProcess();			// 着地処理
	void JumpProcess();				// ジャンプ処理
	void CrouchProcess();			// しゃがみ処理

	// デバッグ用
	void DrawCapsuleCollision();	// カプセルコリジョン描画
	void DrawStatus();				// ステータス描画

protected:

	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// デバッグ用
	bool	_bViewCollision;

	// 固有変数追加
	float _fVelY;			// Y方向の速度
	bool _bIsJumping;		// ジャンプ中かどうか
	bool _bIsStanding;		// 着地しているかどうか
	bool _bIsCrouching;		// しゃがんでいるかどうか
	bool _bIsStartCrouch;	// しゃがみ開始フラグ
};

