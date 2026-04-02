/*****************************************
* file   PlayerShieldSystem.h
* brief  プレイヤーのシールドシステムクラス
* author 成田 悠真
* date   2026/02/08
******************************************/

#pragma once
#include "appframe.h"
#include "ShieldBase.h"

/// @brief プレイヤーのシールドシステムクラス
class PlayerShieldSystem
{
public:

	PlayerShieldSystem();
	virtual ~PlayerShieldSystem();


	/* 基本関数 */

	bool Initialize();			// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	// シールド処理関数

	/// @brief シールドの開始処理関数
	///
	/// @param key シールド開始の入力キー
	void ProcessInput(int key);

	/// @brief シールドがアクティブかチェックする関数
	///
	/// @return シールドがアクティブならtrue、そうでないならfalse
	bool IsActive() const;

	/* 設定関係 */

	/// @brief シールドの所有者を設定する関数
	///
	/// @param owner 設定する所有者のCharaBaseへの共有ポインタ
	void SetOwner(std::shared_ptr<CharaBase> owner);

	/// @brief シールドの設定を行う関数
	///
	/// @param config シールド設定構造体
	void SetShieldConfig(const ShieldConfig& config);

	/// @brief シールドベースクラスへのアクセス関数
	///
	/// @return シールドベースクラスへの共有ポインタ
	std::shared_ptr<ShieldBase> GetShieldBase() const { return _shieldBase; }

protected:

	std::shared_ptr<ShieldBase> _shieldBase;	// シールドベースクラス

};

