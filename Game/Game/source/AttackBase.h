// 担当 : 成田

#pragma once
#include "appframe.h"
#include "GeometryUtility.h"

#include "AttackEffectSystem.h"

  // 前方宣言
class CharaBase;

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

// 攻撃の状態を管理
enum class ATTACK_STATE
{
	INACTIVE,	// 非アクティブ
	STARTUP,	// 発生前
	ACTIVE,		// 攻撃判定中
	RECOVERY,	// 硬直中
	_EOT_,
};

// 攻撃コリジョン情報構造体
struct AttackCollision
{
	VECTOR attackColTop;		// カプセル攻撃コリジョンの上部
	VECTOR attackColBottom;		// カプセル攻撃コリジョンの下部
	float attackColR;			// カプセル攻撃コリジョンの半径
	VECTOR attackDir;			// 攻撃方向
	float attackDelay;			// 攻撃発生までの遅延時間
	float attackDuration;		// 攻撃の持続時間
	float attackRecovery;		// 攻撃後の後隙
	float damage;				// ダメージ量
	ATTACK_STATE attackState;	// 攻撃状態
	float attackMoveSpeed;		// 攻撃中の移動速度
	bool isActive;				// 現在アクティブかどうか
	bool isHit = false;			// ヒットしたかどうか(共通初期化)
	bool canKnockback;			// 吹き飛ばし攻撃かどうか
	bool isAttackCancelByHit;	// 被弾でキャンセルされる攻撃か
};

// 攻撃コリジョンオフセット情報構造体
struct AttackColOffset
{
	float directionScale;   // 方向スケール
	bool useOwnerDirection; // 所有者の向きを基準とするか
};

// 攻撃移動情報構造体
struct AttackMovement
{
	VECTOR moveDir;		// 移動方向
	float moveDistance;	// 移動距離
	float moveSpeed;	// 移動速度
	float decayRate;	// 減衰率
	bool canMove;		// 移動可能フラグ
};

// 攻撃のベースクラス
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

	// 攻撃中の移動処理
	void UpdateAttackMove();				// 移動更新
	virtual void ProcessAttackMovement();	// 移動処理

	// 攻撃コリジョンの位置更新
	void UpdateAttackColPos();	

	// 攻撃コリジョンの位置計算(コリジョン上下のオフセットを入れる場合の計算用関数).未使用
	VECTOR CalculateAttackColPos(const VECTOR& basePos, const VECTOR& offset, const VECTOR& direction);	

	// 攻撃の向き調整処理
	void UpdateAttackDirAdjust();	// 向き調整更新

	// 当たったキャラ管理
	void AddHitCharas(std::shared_ptr<CharaBase> chara);		// 当たったキャラを追加
	bool HasHitCharas(std::shared_ptr<CharaBase> chara)const;	// 当たったキャラを持っているかチェック
	void ClearHitCharas();										// 当たったキャラリストクリア

	// デバッグ表示
	void DrawAttackCollision();

	// カプセル攻撃データ設定
	void SetCapsuleAttackData
	(
		VECTOR top,					// カプセル上部
		VECTOR bottom,				// カプセル下部
		float radius,				// 半径
		VECTOR attackDir,			// 攻撃方向
		float delay,				// 発生遅延
		float duration,				// 持続時間
		float recovery,				// 後隙
		float damage,				// ダメージ	
		ATTACK_STATE attackState,	// 攻撃状態
		float attackMoveSpeed,		// 攻撃中の移動速度
		bool canKnockback			// 吹き飛ばし攻撃かどうか
	);

	/// @brief カプセル攻撃データ設定関数
	///
	/// @param data 攻撃コリジョン情報構造体
	void SetCapsuleAttackData(const AttackCollision& data);

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

	// 攻撃コリジョンオフセット設定
	void SetCollisionOffset(const AttackColOffset& offset);

	// 向き調整データ設定
	void SetDirAdjustData(bool canAdjust);

	// ゲッターセッター
	COLLISION_TYPE GetCollisionType() const { return _eColType; }			// コリジョンタイプ取得
	AttackCollision GetAttackCollision() const { return _stcAttackCol; }	// 攻撃コリジョン情報取得
	ATTACK_STATE GetAttackState() const { return _eAttackState; }			// 攻撃状態取得

	bool GetHitFlag() const { return _stcAttackCol.isHit; }		// ヒットフラグ取得
	void SetHitFlag(bool hit) { _stcAttackCol.isHit = hit; }	// ヒットフラグ設定

	float GetDamage() const { return _stcAttackCol.damage; }		// ダメージ取得
	void SetDamage(float damage) { _stcAttackCol.damage = damage; }	// ダメージ設定

	std::shared_ptr<CharaBase> GetOwner() const { return _owner.lock(); }	// 所有者取得
	void SetOwner(std::shared_ptr<CharaBase> owner) { _owner = owner; }		// 所有者設定

	const AttackEffectConfig& GetAttackEffectConfig() const { return _attackEffectConfig; }			// 攻撃エフェクト情報取得
	void SetAttackEffectConfig(const AttackEffectConfig& config) { _attackEffectConfig = config; }	// 攻撃エフェクト情報設定

protected:

	std::weak_ptr<CharaBase> _owner;	// 所有者キャラ

	// 状態関係
	COLLISION_TYPE _eColType;		// コリジョンタイプ
	ATTACK_STATE _eAttackState;		// 攻撃状態

	// 攻撃情報関係
	AttackCollision		_stcAttackCol;		// 攻撃コリジョン情報
	AttackColOffset		_stcColOffset;		// 攻撃コリジョンオフセット情報
	AttackMovement		_stcAttackMovement;	// 攻撃中の移動情報

	// 攻撃時の向き調整関係
	float _dirAdjustSpeed;	// 向き調整速度
	bool _canDirAdjust;		// 向き調整可能フラグ

	float _fCurrentTime;	// 経過時間	

private:

	std::vector<std::shared_ptr<CharaBase>> _hitChars;	// 当たったキャラを管理

	// 攻撃エフェクト情報
	AttackEffectConfig _attackEffectConfig;	

	// 入力方向計算関数
	VECTOR CalculateInputDir(const AnalogState& analog);

	VECTOR _originalColTop;		// 元のコリジョン上部位置
	VECTOR _originalColBottom;	// 元のコリジョン下部位置


};

