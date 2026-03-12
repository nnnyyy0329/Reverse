#pragma once
#include "appframe.h"

/// @brief アビリティアクションヒントクラス
class AbilityActionHint
{
public:
	AbilityActionHint();
	~AbilityActionHint();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

private:

	/// @brief アクションヒントの表示
	void ActionHintRender();

protected:

	int _iHandleAbsorbAction;			// 吸収攻撃アクションヒントのハンドル
	int _iHandleAoeAction;				// 範囲攻撃アクションヒントのハンドル
	int _iHandlePunchAction;			// 打撃攻撃アクションヒントのハンドル
	int _iHandlePiercingBulletAction;	// 貫通弾アクションヒントのハンドル
	int _iHandleNormalBulletAction;		// 通常弾アクションヒントのハンドル

};

