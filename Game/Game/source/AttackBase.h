// 担当 : 成田

#pragma once
#include "appframe.h"
#include "EnergyManager.h"

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
	bool isHit;				// ヒットしたかどうか
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

	// 攻撃処理
	virtual bool ProcessStartAttack();	// 攻撃開始
	virtual bool ProcessStopAttack();	// 攻撃停止
	void UpdateAttackState();			// 攻撃状態更新

	// 当たったキャラ管理
	void AddHitCharas(std::shared_ptr<CharaBase> chara);		// 当たったキャラを追加
	bool HasHitCharas(std::shared_ptr<CharaBase> chara)const;	// 当たったキャラを持っているかチェック
	void ClearHitCharas();										// 当たったキャラリストクリア

	// カプセル攻撃データ設定
	void SetCapsuleAttackData
	(
		VECTOR top,		// カプセル上部
		VECTOR bottom,	// カプセル下部
		float radius,	// 半径
		float delay,	// 発生遅延
		float duration, // 持続時間
		float recovery, // 後隙
		float damage,	// ダメージ	
		bool hit		// ヒットフラグ
	);

	// 円形攻撃データ設定
	void SetCircleAttackData
	(
		VECTOR center,	// 中心位置
		float radius,	// 半径
		float height,	// 高さ
		float delay,	// 発生遅延
		float duration,	// 持続時間
		float recovery,	// 後隙
		float damage,	// ダメージ
		bool hit		// ヒットフラグ
	);

	// 球攻撃データ設定
	void SetSphereAttackData
	(
		VECTOR center,	// 中心位置
		float radius,	// 半径
		float delay,	// 発生遅延
		float duration,	// 持続時間
		float recovery,	// 後隙
		float damage,	// ダメージ
		bool hit		// ヒットフラグ
	);

	void DrawAttackCollision();	// 攻撃コリジョン表示

	// ゲッターセッター
	COLLISION_TYPE GetCollisionType() const { return _eColType; }			// コリジョンタイプ取得
	ATTACK_COLLISION GetAttackCollision() const { return _stcAttackCol; }	// 攻撃コリジョン情報取得
	ATTACK_STATE GetAttackState() const { return _eAttackState; }			// 攻撃状態取得

	bool GetHitFlag() const { return _stcAttackCol.isHit; }		// ヒットフラグ取得
	void SetHitFlag(bool hit) { _stcAttackCol.isHit = hit; }	// ヒットフラグ設定

	float GetDamage() const { return _stcAttackCol.damage; }		// ダメージ取得
	void SetDamage(float damage) { _stcAttackCol.damage = damage; }	// ダメージ設定

protected:
	// 攻撃コリジョン関係
	COLLISION_TYPE _eColType;		// コリジョンタイプ
	ATTACK_COLLISION _stcAttackCol;	// 攻撃コリジョン情報
	ATTACK_STATE _eAttackState;		// 攻撃状態

private:
	std::shared_ptr<EnergyManager> _energyManager;	// エネルギーマネージャー
	std::vector<std::shared_ptr<CharaBase>> _hitCharas;	// 当たったキャラを管理

};

