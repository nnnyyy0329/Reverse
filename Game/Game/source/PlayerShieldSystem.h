#pragma once
#include "ShieldBase.h"
#include "appframe.h"

class PlayerBase;

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

	void InitializeShieldData();	// シールドデータ初期化

	// シールド処理関数
	void CallShield();
	void ProcessShieldInput();
	int GetPlayerInput()const;

	// プレイヤー設定
	void SetPlayer(PlayerBase* player){ _playerBase = player; }

protected:
	virtual ShieldConfig GetShieldConfig();	// 攻撃定数を取得

	PlayerBase* _playerBase;
	std::shared_ptr<ShieldBase>_shieldBase;

};

