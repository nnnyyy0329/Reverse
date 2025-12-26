// 担当 : 成田

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

	void ProcessMovePlayer();		// プレイヤー移動処理
	void ProcessStatusAnimation();	// ステータスに応じたアニメーション処理
	void ProcessPlayAnimation();	// アニメーション再生処理
	void ProcessStanding();			// 着地処理
	void ProcessJump();				// ジャンプ処理
	void ProcessCrouch();			// しゃがみ処理
	void ProcessDeath();			// 死亡処理
	void ProcessDebug();			// デバッグ用関数

	void DrawModel();				// モデル描画
	void DrawDebug();				// デバッグ用表示
	void DrawCoordinate();			// 座標関係の表示
	void DrawCapsuleCollision();	// カプセルコリジョン表示
	void DrawStatus();				// ステータス表示
	void DrawParameter();			// パラメーター表示

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

	// 表示用オフセット
	int _drawSizeOffset;
	int _drawOffsetX;
	int _drawOffsetY;
};

