// 担当 : 成田

#pragma once
#include "appframe.h"

// エネルギー情報定数
namespace EnergyDefaultConstants
{
    constexpr float MIN_ENERGY = 0.0f;				// 最小エネルギー量
    constexpr float MAX_ENERGY = 300.0f;			// 最大エネルギー量
    constexpr float CONVERT_MULTIPLIER = 1.0f;		// デフォルト変換倍率
    constexpr float CONSUME_MULTIPLIER = 1.0f;		// デフォルト消費倍率
    constexpr float SWITCH_COST_ENERGY = 100.0f;	// 切り替え可能エネルギー
    constexpr float SWITCH_KEEP_ENERGY = 0.01f;		// 切り替え維持エネルギー
}

// エネルギー獲得量定数
namespace EnergyGainConstants
{
	constexpr float EVADE_ATTACK = 25.0f;   // 攻撃回避時のエネルギー獲得量
	constexpr float EVADE_BULLET = 20.0f;   // 弾回避時のエネルギー獲得量
	constexpr float ABSORB = 1.0f;          // 吸収時のエネルギー獲得量
}

// エネルギー消費量定数
namespace EnergyConsumeConstants
{
	constexpr float SHOOT_NORMAL_BULLET = 10.0f;	    // 通常弾のエネルギー消費量
	constexpr float SHOOT_PIERCING_BULLET = 5.0f;	// 貫通弾のエネルギー消費量
}

/// @brief エネルギーマネージャークラス
class EnergyManager
{
public:

    // シングルトン
    static EnergyManager* GetInstance()	// インスタンス取得
    {
        // 静的ローカル変数シングルトン
        static EnergyManager instance;
        return &instance;
    }
    static void CreateInstance();	// インスタンス作成
    static void DestroyInstance();	// インスタンス破棄


	/* 基本関数 */

    bool Initialize();
    bool Terminate();
    bool Process();
    bool Render();

	/// @brief デバッグ描画
    void DebugRender();


    /* エネルギー管理関数 */

	/// @brief ダメージをエネルギーに変換する関数
    ///
	/// @param damage ダメージ量
    void ConvertDamageToEnergy(float damage);

	/// @brief ダメージを消費エネルギーに変換する関数
	///
	/// @param damage ダメージ量
    void ConvertDamageToConsumeEnergy(float damage);

	/// @brief エネルギー追加関数
	///
	/// @param energy 追加するエネルギー量
    void AddEnergy(float Energy);                   

	/// @brief エネルギー消費関数
	///
	/// @param energy 消費するエネルギー量
    void ConsumeEnergy(float energy);

	/// @brief 切り替え可能かチェック
    ///
	/// @return 切り替え可能ならtrue、そうでなければfalse
	bool CanSwitchPlayer() const { return _currentEnergy >= _switchCostEnergy; }

	/// @brief 切り替え維持可能かチェック
	///
	/// @return 切り替え維持可能ならtrue、そうでなければfalse
	bool CanKeepSwitchPlayer() const { return _currentEnergy >= _switchKeepEnergy; }

	/// @brief エネルギーが最大まで達しているかチェック
	///
	/// @return エネルギーが最大まで達しているならtrue、そうでなければfalse
	bool IsMaxEnergy() const { return _currentEnergy >= _maxEnergy; }


    /* ゲッターセッター */

	/// @brief 現在のエネルギーの取得
	///
	/// @return 現在のエネルギー量
    float GetCurrentEnergy() const { return _currentEnergy; }       

	/// @brief 現在のエネルギーの設定
	///
	/// @param energy 設定するエネルギー量
    void SetCurrentEnergy(float energy){ _currentEnergy = energy; } 

	/// @brief 最大のエネルギーの取得
	///
	/// @return 最大のエネルギー量
    float GetMaxEnergy() const { return _maxEnergy; }       

	/// @brief 切り替えに必要なエネルギーの取得
    /// 
	/// @return 切り替えに必要なエネルギー量
    float GetSwitchCostEnergy() const { return _switchCostEnergy; } 

	/// @brief 切り替え状態維持に必要なエネルギーの取得
	///
	/// @return 切り替え状態維持に必要なエネルギー量
	float GetSwitchKeepEnergy() const { return _switchKeepEnergy; }   

	/// @brief 攻撃回避時のエネルギー獲得量の取得
	///
	/// @return 攻撃回避時のエネルギー獲得量
	float GetEvadeAttackEnergy() const { return EnergyGainConstants::EVADE_ATTACK; }

	/// @brief 弾回避時のエネルギー獲得量の取得
	///
	/// @return 弾回避時のエネルギー獲得量
	float GetEvadeBulletEnergy() const { return EnergyGainConstants::EVADE_BULLET; }

	/// @brief 吸収時のエネルギー獲得量の取得
	///
	/// @return 吸収時のエネルギー獲得量
	float GetAbsorbEnergy() const { return EnergyGainConstants::ABSORB; }

	/// @brief 通常弾のエネルギー消費量の取得
	///
	/// @return 通常弾のエネルギー消費量
	float GetNormalBulletEnergyCost() const { return EnergyConsumeConstants::SHOOT_NORMAL_BULLET; }

	/// @brief 貫通弾のエネルギー消費量の取得
	///
	/// @return 貫通弾のエネルギー消費量
	float GetPiercingBulletEnergyCost() const { return EnergyConsumeConstants::SHOOT_PIERCING_BULLET; }


private:
    
    // コンストラクタ、デストラクタ
    EnergyManager();
    virtual ~EnergyManager();

    // シングルトン用変数
    static EnergyManager* _instance;

protected:

    float _currentEnergy;               // 現在のエネルギー量
    float _maxEnergy;                   // 最大のエネルギー量
	float _convertMultiplier;           // ダメージからエネルギーへの変換倍率
	float _consumeConvertMultiplier;    // ダメージから消費エネルギーへの変換倍率
    float _switchCostEnergy;            // 切り替えに必要なエネルギー量
	float _switchKeepEnergy;            // 切り替え状態維持に必要なするエネルギー量

};

