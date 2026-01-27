#include "HitCheckManager.h"
#include "CharaBase.h"

// コンストラクタ
template<typename ObjectName>
HitCheckManager<ObjectName>::HitCheckManager()
{

}

// デストラクタ
template<typename ObjectName>
HitCheckManager<ObjectName>::~HitCheckManager()
{

}

// 初期化
template<typename ObjectName>
bool HitCheckManager<ObjectName>::Initialize()
{
	return true;
}

// 解放処理
template<typename ObjectName>
bool HitCheckManager<ObjectName>::Terminate()
{
	return true;
}

// 更新処理
template<typename ObjectName>
bool HitCheckManager<ObjectName>::Process()
{
	return true;
}

// 描画処理
template<typename ObjectName>
bool HitCheckManager<ObjectName>::Render()
{
	return true;
}

// カプセル同士の当たり判定（グローバルテンプレート関数）
template<typename ObjectName>
template<typename Obj1, typename Obj2>
bool HitCheckManager<ObjectName>::HitCheckCapsuleCapsule(const Obj1& object1, const Obj2& object2)
{
	if(object1 == nullptr || object2 == nullptr){ return false; }

	if(HitCheck_Capsule_Capsule
	(
		object1->GetCollisionTop(),object1->GetCollisionBottom(),object1->GetCollisionR(),
		object2->GetCollisionTop(),object2->GetCollisionBottom(),object2->GetCollisionR()
	) != false)
	{
		return true;
	}

	return false;
}