// 担当 : 成田

#pragma once
#include "appframe.h"
#include "AttackBase.h"
#include "EnergyManager.h"

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

/// @brief 攻撃管理クラス
class AttackManager
{
public:

	// シングルトン
	static AttackManager* GetInstance()	// インスタンス取得
	{
		static AttackManager instance;	// 静的インスタンス
		return &instance;				// インスタンスを返す
	}
	static void CreateInstance();			// インスタンス作成
	static void DestroyInstance();			// インスタンス破棄

	bool Initialize();		
	bool Terminate();		
	bool Process();			
	bool Render();			

	// デバッグ情報描画
	void DebugRender();		

	// コリジョン描画
	void CollisionRender(); 


	/* 攻撃の登録と解除 */

	/// @brief 攻撃の登録関数
	///
	/// @param attack 登録する攻撃オブジェクト
	/// @param ownerType 攻撃の所有者タイプ
	/// @param ownerId 攻撃の所有者ID
	void RegisterAttack(std::shared_ptr<AttackBase> attack, ATTACK_OWNER_TYPE ownerType, int ownerId);

	/// @brief 回避成功時の処理
	void ProcessEvadeSuccess();

	/// @brief 攻撃の解除関数
	///
	/// @param attack 解除する攻撃オブジェクト
	void UnregisterAttack(std::shared_ptr<AttackBase> attack);	

	/// @brief 所有者による攻撃解除関数
	///
	/// @param ownerId 解除する攻撃の所有者ID
	void UnregisterAttackByOwner(int ownerId);

	/// @brief 全ての攻撃解除関数
	void ClearAllAttacks();	

	/// @brief 無効な攻撃のクリーンアップ関数
	void CleanupInvalidAttacks();	

	/// @brief 攻撃が登録されているかチェック関数
	///
	/// @param attack チェックする攻撃オブジェクト
	bool IsAttackRegistered(std::shared_ptr<AttackBase> attack)const;	

	/// @brief 特定の攻撃移動を停止
	///
	/// @param attack 停止対象の攻撃
	bool StopAttackMovementByAttack(std::shared_ptr<AttackBase> attack);

	/// @brief 特定の所有者の攻撃移動を停止
	///
	/// @param ownerType 停止対象の所有者タイプ
	bool StopAttackMovementByOwner(ATTACK_OWNER_TYPE ownerType);

	/* 回避にヒットした攻撃の登録と解除 */

	/// @brief 回避にヒットした攻撃の登録関数
	///
	/// @param attack 登録する攻撃オブジェクト
	void RegisterDodgeHitAttack(std::shared_ptr<AttackBase> attack);

	/// @brief 回避にヒットした攻撃のクリア関数
	void ClearDodgeHitAttacks();										

	/// @brief 回避にヒットした攻撃のクリーンアップ関数
	void CleanupDodgeHitAttacks();										

	/// @brief 回避にヒットした攻撃かチェック関数
	///
	/// @param attack チェックする攻撃オブジェクト
	bool IsDodgeHitAttack(std::shared_ptr<AttackBase> attack) const;	


	/* 攻撃の取得 */

	/// @brief 全てのアクティブな攻撃を取得関数
	///
	/// @return 登録されたすべてのアクティブな攻撃のポインタのベクター
	std::vector<std::shared_ptr<AttackBase>>GetAllActiveAttacks()const;									

	/// @brief 所有者タイプによる攻撃取得関数
	///
	/// @param ownerType 取得する攻撃の所有者タイプ
	/// 
	/// @return 指定された所有者タイプのすべての攻撃の共有ポインタのベクター
	std::vector<std::shared_ptr<AttackBase>>GetAttacksByOwnerType(ATTACK_OWNER_TYPE ownerType)const;	

	/// @brief 所有者IDによる攻撃取得関数
	///
	/// @param ownerId 取得する攻撃の所有者ID
	std::vector<std::shared_ptr<AttackBase>> GetAttacksByOwnerId(int ownerId) const;						


	/* ゲッターセッター */

	/// @brief 登録された攻撃数の取得関数
	///
	/// @return 登録された攻撃の数
	int GetRegisteredAttackCount() const { return static_cast<int>(_registeredAttacks.size()); }	

	/// @brief アクティブな攻撃数の取得関数
	///
	/// @return アクティブな攻撃の数
	int GetActiveAttackCount() const;																

	/// @brief フレームカウンタの取得関数
	///
	/// @return 現在のフレームカウンタの値
	int GetFrameCounter() const { return _frameCounter; }											

	/// @brief 攻撃の所有者タイプの取得関数
	///
	/// @param attack 所有者タイプを取得する攻撃オブジェクト
	///	
	/// @return 指定された攻撃の所有者タイプ
	ATTACK_OWNER_TYPE GetAttackOwnerType(std::shared_ptr<AttackBase> attack) const;	

	/// @brief 攻撃の所有者IDの取得関数
	///
	/// @param attack 所有者IDを取得する攻撃オブジェクト
	/// 
	/// @return 指定された攻撃の所有者ID
	int GetAttackOwnerId(std::shared_ptr<AttackBase> attack) const;					

private:

	// シングルトン用コンストラクタ、デストラクタ
	AttackManager();
	virtual ~AttackManager();

	// シングルトン用メンバ
	static AttackManager* _instance;

	/// @brief 攻撃による移動の停止を通知する関数
	bool NotifyStopMovementByAttack();

protected:
	std::vector<std::weak_ptr<AttackBase>> _dodgeHitAttacks;	// 回避にヒットした攻撃リスト
	std::vector<ATTACK_INFO> _registeredAttacks;				// 登録された攻撃リスト

	int _frameCounter;	// フレームカウンタ

};

