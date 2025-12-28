// 作成 : 成田

#pragma once
#include "appframe.h"

enum class COLLISION_TYPE
{
	NONE,		// なし
	CAPSULE,	// カプセル
	CIRCLE,		// 円
	SPHERE,		// 球
	BOX,		// 矩形
	POLYGON,	// ポリゴン
	_EOT_,		// 終端
};

// 攻撃コリジョン情報
struct ATTACK_COLLISION
{
	VECTOR attackColTop;	// カプセル攻撃コリジョンの上部
	VECTOR attackColBottom;	// カプセル攻撃コリジョンの下部
	float attackColR;		// カプセル攻撃コリジョンの半径
	float attackDelay;		// 攻撃発生までの遅延時間
	float attackDuration;	// 攻撃の持続時間
	float recovery;			// 攻撃後の後隙
	float damage;			// ダメージ量
	float currentTime;		// 現在の経過時間
	bool isActive;			// 現在アクティブかどうか
};

// 攻撃の状態を管理
enum class ATTACK_STATE
{
	INACTIVE,	// 非アクティブ
	STARTUP,	// 発生前
	ACTIVE,		// 攻撃判定中
	RECOVERY,	// 硬直中
	_EOT_,
};

class CharaBase;  // 前方宣言

class AttackBase
{
public:
	AttackBase();
	virtual ~AttackBase();

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();
	virtual bool ProcessStartAttack();	// 攻撃開始
	virtual bool ProcessStopAttack();	// 攻撃停止
	void UpdateAttackState();			// 攻撃状態更新

	// カプセル攻撃データ設定
	void SetCapsuleAttackData
	(
		VECTOR top, 
		VECTOR bottom, 
		float radius, 
		float delay, 
		float duration, 
		float recovery, 
		float damage
	);

	// 円形攻撃データ設定
	void SetCircleAttackData
	(
		VECTOR center,
		float radius,
		float height,
		float delay,
		float duration,
		float recovery,
		float damage
	);

	// 球攻撃データ設定
	void SetSphereAttackData
	(
		VECTOR center,
		float radius,
		float delay,
		float duration,
		float recovery,
		float damage
	);

	void DrawDebug();	// デバッグ表示

	// ゲッターセッター
	COLLISION_TYPE GetCollisionType() const { return _eColType; }
	ATTACK_COLLISION GetAttackCollision() const { return _stcAttackCol; }
	ATTACK_STATE GetAttackState() const { return _eAttackState; }

protected:
	// 攻撃コリジョン関係
	COLLISION_TYPE _eColType;
	ATTACK_COLLISION _stcAttackCol;
	ATTACK_STATE _eAttackState;
};

