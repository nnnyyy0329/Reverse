#pragma once
#include "appframe.h"

// ヒットチェック管理のためのクラス
template<typename ObjectName>
class HitCheckManager
{
public:
	HitCheckManager();
	~HitCheckManager();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();

	// カプセル同士の当たり判定（グローバルテンプレート関数）
	template<typename Obj1, typename Obj2>
	bool HitCheckCapsuleCapsule(const Obj1& object1, const Obj2& object2);

	// カプセルと球の当たり判定（グローバルテンプレート関数）
	template<typename Obj1, typename Obj2>
	bool HitCheckCapsuleSphere(const Obj1& object1, const Obj2& object2);

	// 球同士の当たり判定（グローバルテンプレート関数）
	template<typename Obj1, typename Obj2>
	bool HitCheckSphereSphere(const Obj1& object1, const Obj2& object2);


	// カプセル同士の当たり判定（静的テンプレート関数）
	template<typename Obj1, typename Obj2>
	bool HitCheckCapsuleCapsule(const Obj1& object1, const Obj2& object2);


private:

protected:

};

