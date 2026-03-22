#pragma once
#include "appframe.h"

/// @brief 弾のエフェクト情報構造体
struct BulletEffectConfig
{
	// エフェクト
	std::string effectName;	// エフェクト名
	VECTOR effectOffset;	// エフェクトのオフセット

	// サウンド
	std::string soundName;	// サウンド名
};

/// @brief 弾のエフェクトシステムのシングルトンクラス
class BulletEffectSystem
{
public:

	/* シングルトンアクセス */

	/// @brief インスタンス取得処理
	static BulletEffectSystem* GetInstance();

	/// @brief インスタンス生成処理
	static void CreateInstance();

	/// @brief インスタンス破棄処理
	static void DestroyInstance();


	/* 基本処理 */

	bool Initialize();
	bool Terminate();
	bool Process();

	/// @brief エフェクト再生
	///
	/// @param effectName エフェクト名
	/// @param pos 再生位置
	/// 
	/// @return エフェクトハンドル
	int PlayBulletEffect(const std::string& effectName, const VECTOR& pos);

	/// @brief エフェクト位置更新
	///
	/// @param effectHandle エフェクトハンドル
	/// @param pos 新しい位置
	void UpdateBulletEffectPos(int effectHandle, const VECTOR& pos);

	/// @brief エフェクト停止
	///
	/// @param effectHandle エフェクトハンドル
	void StopBulletEffect(int effectHandle);

	/// @brief サウンド再生
	///
	/// @param soundName サウンド名
	void PlayBulletSound(const std::string& soundName);

private:

	// コンストラクタ、デストラクタ
	BulletEffectSystem();
	virtual ~BulletEffectSystem();

	// シングルトンインスタンス
	static BulletEffectSystem* _instance;

	//// アクティブなエフェクト管理マップ
	//std::map<int, bool> _activeEffects;

};

