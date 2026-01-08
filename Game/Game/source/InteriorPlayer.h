// 担当 : 成田

#pragma once
#include "PlayerBase.h"
#include "AttackBase.h"

class InteriorPlayer : public PlayerBase
{
public:
	InteriorPlayer();
	virtual ~InteriorPlayer();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	// SurfacePlayer_Move.cppで定義
	void CallProcess();				// 移動関係Process呼び出し用関数
	void ProcessMovePlayer();		// プレイヤー移動処理
	void ProcessStatusAnimation();	// ステータスに応じたアニメーション処理
	void ProcessPlayAnimation();	// アニメーション再生処理
	void ProcessStanding();			// 着地処理
	void ProcessJump();				// ジャンプ処理
	void ProcessCrouch();			// しゃがみ処理
	void ProcessDeath();			// 死亡処理
	void ProcessDebug();			// デバッグ用関数

	// SurfacePlayer_Draw.cppで定義
	void CallDraw();				// 描画関係の呼び出し用関数
	void DrawModel();				// モデル表示
	void DrawBaseData();			// 基礎情報表示
	void DrawCoordinate();			// 座標の表示
	void DrawCapsuleCollision();	// カプセルコリジョン表示
	void DrawStatus();				// ステータス表示
	void DrawParameter();			// パラメーター表示
	void DrawAnimationName();		// 再生されているアニメーション名表示
	void DrawColPos();				// コリジョンの情報表示
	void DrawAttackColData();		// 受け取ったコリジョンのデータ表示

protected:


};

