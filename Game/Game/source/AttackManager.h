// 担当 : 成田

#pragma once
#include "appframe.h"
#include "AttackBase.h"

enum class ATTACK_OWNER_TYPE
{
	NONE,
	SURFACE_PLAYER,		// 表プレイヤー
	INTERIOR_PLAYER,	// 裏プレイヤー
	ENEMY,				// 敵
	_EOT_,
};

// 攻撃情報構造体
struct ATTACK_INFO
{
	std::weak_ptr<AttackBase> attack;	// 攻撃オブジェクト
	ATTACK_OWNER_TYPE ownerType;		// 所有者タイプ
	int ownerId;						// 所有者ID
	int registrationFrame;				// 登録されたフレーム数
};

class AttackManager
{
public:
	AttackManager();
	virtual ~AttackManager();

	bool Initialize();		// 初期化
	bool Terminate();		// 終了
	bool Process();			// 更新
	bool Render();			// 描画
	void DebugRender();		// デバッグ情報描画
	void CollisionRender(); // コリジョン描画

	// 攻撃の登録と解除
	void RegisterAttack(std::shared_ptr<AttackBase> attack, ATTACK_OWNER_TYPE ownerType, int ownerId);	// 攻撃登録
	void UnregisterAttack(std::shared_ptr<AttackBase> attack);											// 攻撃解除
	void UnregisterAttackByOwner(int ownerId);															// 所有者による攻撃解除
	void ClearAllAttacks();																				// 全ての攻撃解除
	void CleanupInvalidAttacks();																		// 無効な攻撃のクリーンアップ
	bool IsAttackRegistered(std::shared_ptr<AttackBase> attack)const;									// 攻撃が登録されているかチェック

	// 回避にヒットした攻撃の登録と解除
	void RegisterDodgeHitAttack(std::shared_ptr<AttackBase> attack);	// 回避にヒットした攻撃を登録
	void ClearDodgeHitAttacks();										// 回避にヒットした攻撃をクリア
	void CleanupDodgeHitAttacks();										// 回避にヒットした攻撃のクリーンアップ
	bool IsDodgeHitAttack(std::shared_ptr<AttackBase> attack) const;	// 回避にヒットした攻撃かチェック

	// 攻撃の取得
	std::vector<std::shared_ptr<AttackBase>>GetAllActiveAttacks()const;									// 全てのアクティブな攻撃を取得
	std::vector<std::shared_ptr<AttackBase>>GetAttacksByOwnerType(ATTACK_OWNER_TYPE ownerType)const;	// 所有者タイプによる攻撃取得
	std::vector<std::shared_ptr<AttackBase>> GetAttacksByOwner(int ownerId) const;						// 所有者IDによる攻撃取得

	// ゲッターセッター
	int GetRegisteredAttackCount() const { return static_cast<int>(_registeredAttacks.size()); }	// 登録攻撃数取得
	int GetActiveAttackCount() const;																// アクティブ攻撃数取得
	int GetFrameCounter() const { return _frameCounter; }											// フレームカウンタ取得

	ATTACK_OWNER_TYPE GetAttackOwnerType(std::shared_ptr<AttackBase> attack) const;	// 攻撃の所有者を取得
	int GetAttackOwnerId(std::shared_ptr<AttackBase> attack) const;					// 攻撃の所有者IDを取得

	// シングルトン
	static AttackManager* GetInstance()	// インスタンス取得
	{
		// 静的ローカル変数シングルトン
		static AttackManager instance;

		return &instance;
	}
	static void CreateInstance();	// インスタンス作成
	static void DestroyInstance();	// インスタンス破棄

protected:
	std::vector<std::weak_ptr<AttackBase>> _dodgeHitAttacks;	// 回避にヒットした攻撃リスト
	std::vector<ATTACK_INFO> _registeredAttacks;				// 登録された攻撃リスト

	int _frameCounter;	// フレームカウンタ

	// シングルトン用メンバ
	static AttackManager* _instance;

};

