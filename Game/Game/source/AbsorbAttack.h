#pragma once
#include "AttackBase.h"

// 吸収量に関する定数
namespace AbsorbAmountConstants
{
	constexpr float ABSORB_ENERGY = 1.0;	// 吸収エネルギー量
	constexpr float ABSORB_HP = 1.0f;		// 吸収HP量
}

// 吸収時間に関する定数
namespace AbsorbTimeConstants
{
	constexpr float ABSORB_UP_AMOUNT_TIME	= 300.0f;	// 吸収量が上昇するまでの時間
	constexpr float ABSORB_COOLDOWN			= 30.0f;	// 吸収のクールダウン時間
	constexpr float DECREMENT_ABSORB_TIMER	= 1.0f;		// 吸収タイマーの減算量
}

// 吸収攻撃の設定構造体
struct AbsorbConfig
{
	float absorbRate;				// 吸収率
	float energyAbsorbRate;			// エネルギー吸収率
	float hpAbsorbRate;				// HP吸収率
	float absorbRange;				// 吸収範囲
	float absorbAngle;				// 吸収範囲の角度
	int absorbDivision;				// 吸収範囲の分割数
	bool isActive;					// 吸収攻撃がアクティブかどうか
	std::string absorbEffectName;	// 吸収エフェクト名
	VECTOR effectOffset;			// エフェクト位置オフセット
	std::string	absorbSoundName;	// 吸収サウンド名
};

/// @brief 吸収攻撃クラス
class AbsorbAttack : public AttackBase
{
public:
	AbsorbAttack();
	virtual ~AbsorbAttack();

	/// @brief 吸収攻撃の更新
	///
	/// @return 更新処理が成功したかどうか
	virtual bool Process() override;

	/// @brief 吸収攻撃の開始処理
	///
	/// @return 開始処理が成功したかどうか
	virtual bool ProcessStartAttack() override;	

	/// @brief 吸収攻撃の停止処理
	///
	/// @return 停止処理が成功したかどうか
	virtual bool ProcessStopAttack() override;	
	

	/* 吸収処理関連 */

	/// @brief 入力による吸収処理
	///
	/// @param key 入力状態（true: 押されている, false: 押されていない）
	void ProcessAbsorbByInput(bool key);

	/// @brief 吸収処理
	///
	/// @param owner 吸収攻撃の所有者キャラクター
	void ProcessAbsorb(std::shared_ptr<CharaBase>owner);

	/// @brief 吸収攻撃がアクティブ状態かどうかをチェック
	///
	/// @return 吸収攻撃がアクティブ状態であればtrue、そうでなければfalse
	bool IsAbsorbActive() const { return GetAttackState() == ATTACK_STATE::ACTIVE; }

	/// @brief 入力が有効かどうかをチェック
	///
	/// @return 入力が有効であればtrue、そうでなければfalse
	bool IsInputActive() const { return _bIsInputActive; }


	/* デバッグ関連 */

	/// @brief 吸収攻撃のデバッグ描画
	void DebugRender();

	/// @brief 吸収範囲の扇形デバッグ描画
	///
	/// @param ownerPos 所有者の位置
	/// @param ownerDir 所有者の向き
	void DrawAbsorbRange(const VECTOR& ownerPos, const VECTOR& ownerDir);

	/// @brief 吸収攻撃の状態更新デバッグ描画
	void DrawAbsorbAttackState();

	/// @brief 吸収攻撃のカウントデバッグ描画
	void DrawAbsorbTimer();


	/* 吸収情報関連 */

	/// @brief 吸収攻撃の設定を保存する
	///
	/// @param config 吸収攻撃の設定構造体
	void SetAbsorbConfig(const AbsorbConfig& config);

	/// @brief 吸収攻撃の設定を取得する	
	///
	/// @return 吸収攻撃の設定構造体
	AbsorbConfig GetAbsorbConfig() const;

private:	// 内部処理

	/// @brief 吸収タイマーの減算処理
	void ProcessDecrementTimer();

	/// @brief エネルギー吸収処理
	///
	/// @param owner 吸収攻撃の所有者キャラクター
	void ProcessEnergyAbsorb(std::shared_ptr<CharaBase> owner);

	/// @brief HP吸収処理
	///
	/// @param owner 吸収攻撃の所有者キャラクター
	void ProcessHPAbsorb(std::shared_ptr<CharaBase> owner);

	/// @brief 吸収エフェクト処理
	///
	/// @param ownerPos 所有者の位置
	void ProcessAbsorbEffect(const VECTOR& ownerPos);

	/// @brief 吸収サウンド処理
	///
	/// @param owner 吸収攻撃の所有者キャラクター
	void ProcessAbsorbSound(std::shared_ptr<CharaBase> owner);

	/// @brief 吸収攻撃の状態更新処理
	void ProcessAbsorbAttackState();

protected:

	AbsorbConfig _stcAbsorbConfig;	// 吸収攻撃の設定
	float _fAbsorbCooldown;			// 吸収のクールダウン時間
	float _fAbsorbTimer;			// 吸収タイマー
	bool _bIsInputActive;			// 入力が有効かどうか
	bool _bIsAbsorbIncreasing;		// 吸収量が上昇するかのフラグ
	
};

