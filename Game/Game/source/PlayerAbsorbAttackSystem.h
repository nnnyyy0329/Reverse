#pragma once
#include "AbsorbAttack.h"
#include "CharaBase.h"

/// @brief プレイヤーの吸収攻撃システムクラス
class PlayerAbsorbAttackSystem
{
public:
	PlayerAbsorbAttackSystem();
	virtual ~PlayerAbsorbAttackSystem();

	virtual bool Initialize(std::weak_ptr<CharaBase> owner);	// 初期化
	virtual bool Terminate();									// 終了
	virtual bool Process();										// 更新
	virtual bool Render();										// 描画

	/// @brief デバッグ描画関数
	void DebugRender();

	/// @brief 吸収攻撃の設定する
	///
	/// @param config 吸収攻撃の設定構造体
	void SetAbsorbConfig(const AbsorbConfig& config);

	/// @brief 吸収攻撃の設定を取得する
	///
	/// @return 吸収攻撃の設定構造体
	AbsorbConfig GetAbsorbConfig() const;

	/// @brief 吸収攻撃の開始処理
	void StartAbsorbAttack();

	/// @brief 吸収攻撃の停止処理
	void StopAbsorbAttack();

	/// @brief 吸収攻撃の入力処理
	void ProcessAbsorbInput();

	/// @brief 吸収攻撃の更新処理
	void ProcessAbsorb();

	/// @brief 吸収攻撃が開始可能かどうかをチェック
	///
	/// @return 吸収攻撃が開始可能であればtrue、そうでなければfalse
	bool CanStartAbsorbAttack() const;

	/// @brief 吸収攻撃がアクティブ状態かどうかをチェック
	///
	/// @return 吸収攻撃がアクティブ状態であればtrue、そうでなければfalse
	bool IsAbsorbActive() const;

private:

	/// @brief 吸収攻撃クラスインスタンスの作成
	void MakeAbsorbAttack();

	/// @brief 初期化時の所有者設定
	///
	/// @param owner 所有者の弱ポインタ
	void InitializeSetOwner(std::weak_ptr<CharaBase> owner);

	/// @brief 吸収攻撃の状態更新処理
	void ProcessAbsorbAttackState();
	

protected:

	std::weak_ptr<CharaBase> _owner;                // 所有者
	std::unique_ptr<AbsorbAttack> _absorbAttack;    // 吸収攻撃オブジェクト

	bool _bIsInitialized;	// 初期化フラグ

};

