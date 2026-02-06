#pragma once
#include "appframe.h"

// シールド設定構造体
struct Shield

// シールドの状態管理
enum class SHIELD_STATE
{
	NONE,
	INACTIVE,	// 非アクティブ
	STARTUP,	// 発生前
	ACTIVE,		// 攻撃判定中
	RECOVERY,	// 硬直中
	_EOT_,
};

// シールド基底クラス
class ShieldBase
{
public:
	ShieldBase();
	virtual ~ShieldBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了	
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画

protected:


};

