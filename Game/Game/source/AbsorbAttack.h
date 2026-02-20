#pragma once
#include "AttackBase.h"

// 吸収攻撃の設定構造体
struct AbsorbConfig
{
	float absorbRate;				// 吸収率
	float energyAbsorbRate;			// エネルギー吸収率
	float hpAbsorbRate;				// HP吸収率
	float absorbRange;				// 吸収範囲
	float absorbAngle;				// 吸収範囲の角度
	int absorbDivision;				// 吸収範囲の分割数
	std::string absorbEffectName;	// 吸収エフェクト名
};

// 吸収攻撃クラス
class AbsorbAttack : public AttackBase
{
public:
	AbsorbAttack();
	virtual ~AbsorbAttack();

	virtual bool Process() override;			// 更新
	virtual bool ProcessStartAttack() override;	// 攻撃開始
	virtual bool ProcessStopAttack() override;	// 攻撃停止

	// デバッグ描画
	void DebugRender();

	// 吸収範囲の扇形描画
	void DrawAbsorbRange(const VECTOR& ownerPos, const VECTOR& ownerDir);
	
	// 入力による吸収処理
	void ProcessAbsorbByInput(int key);

	// 吸収情報を設定する関数
	void SetAbsorbConfig(const AbsorbConfig& config);

	// 吸収攻撃の設定取得
	AbsorbConfig GetAbsorbConfig() const;

	// 吸収処理
	void ProcessAbsorb(std::shared_ptr<CharaBase>owner);

	// 攻撃がアクティブ状態かどうか
	bool IsAbsorbActive() const { return GetAttackState() == ATTACK_STATE::ACTIVE; }

	// 入力が有効かどうか
	bool IsInputActive() const { return _bIsInputActive; }

private:	// 内部処理

	// タイマー更新処理
	void ProcessDecrementTimer();

	// エネルギー吸収処理
	void ProcessEnergyAbsorb(std::shared_ptr<CharaBase> owner);

	// HP吸収処理
	void ProcessHPAbsorb(std::shared_ptr<CharaBase> owner);

	// 吸収エフェクト処理
	void ProcessAbsorbEffect(const VECTOR& ownerPos);

	// 吸収攻撃の状態更新
	void ProcessAbsorbAttackState();

protected:
	AbsorbConfig _stcAbsorbConfig;	// 吸収攻撃の設定
	float _fAbsorbCooldown;			// 吸収のクールダウン時間
	float _fAbsorbTimer;			// 吸収タイマー
	bool _bIsInputActive;			// 入力が有効かどうか
	
};

