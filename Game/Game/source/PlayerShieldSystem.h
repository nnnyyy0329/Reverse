#pragma once
#include "appframe.h"
#include "ShieldBase.h"

// プレイヤーのシールドシステム
class PlayerShieldSystem
{
public:
	PlayerShieldSystem();
	virtual ~PlayerShieldSystem();

	bool Initialize();			// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

	// シールド処理関数
	void ProcessInput(int key); // シールド入力処理

	// 状態チェック
	bool IsActive() const;

	// 設定関係
	void SetOwner(std::shared_ptr<CharaBase> owner);
	void SetShieldConfig(const ShieldConfig& config);

	// シールドベースへのアクセス
	std::shared_ptr<ShieldBase> GetShieldBase() const { return _shieldBase; }

protected:
	std::shared_ptr<ShieldBase> _shieldBase;	// シールドベースクラス

};

