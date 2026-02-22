#pragma once
#include "AbsorbAttack.h"
#include "CharaBase.h"

// プレイヤーの吸収攻撃システムクラス
class PlayerAbsorbAttackSystem
{
public:
	PlayerAbsorbAttackSystem();
	virtual ~PlayerAbsorbAttackSystem();

	virtual bool Initialize(std::weak_ptr<CharaBase> owner);	// 初期化
	virtual bool Terminate();									// 終了
	virtual bool Process();										// 更新
	virtual bool Render();										// 描画

	// デバッグ描画
	void DebugRender();

	// 吸収攻撃の設定
	void SetAbsorbConfig(const AbsorbConfig& config);

	// 吸収攻撃の設定取得
	AbsorbConfig GetAbsorbConfig() const;

	// 吸収攻撃の開始
	void StartAbsorbAttack();

	// 吸収攻撃の停止
	void StopAbsorbAttack();

	// 吸収攻撃の入力処理
	void ProcessAbsorbInput(int key);

	// 吸収の更新処理
	void ProcessAbsorb();

	// 吸収攻撃が開始可能かどうか
	bool CanStartAbsorbAttack() const;

	// 吸収攻撃がアクティブかどうか
	bool IsAbsorbAttacking() const;	

private:
	// 吸収攻撃クラスインスタンスの作成
	void MakeAbsorbAttack();

	// 初期化時の所有者設定
	void InitializeSetOwner(std::weak_ptr<CharaBase> owner);

	// 吸収攻撃の状態更新
	void ProcessAbsorbAttackState();
	

protected:
	std::weak_ptr<CharaBase> _owner;                // 所有者
	std::unique_ptr<AbsorbAttack> _absorbAttack;    // 吸収攻撃オブジェクト

	bool _bIsInitialized;	// 初期化フラグ

};

