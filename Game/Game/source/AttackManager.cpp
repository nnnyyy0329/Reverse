// 担当 : 成田

#include "AttackManager.h"

// シングルトン用メンバ初期化
AttackManager* AttackManager::_instance = nullptr;

AttackManager::AttackManager()
{
	_frameCounter = 0;	// フレームカウンタ初期化
}

AttackManager::~AttackManager()
{

}

bool AttackManager::Initialize()
{
	_registeredAttacks.clear();	// 登録攻撃リストクリア
	_frameCounter = 0;			// フレームカウンタ初期化

	return true;
}

bool AttackManager::Terminate()
{
	ClearAllAttacks();	// 全ての攻撃解除

	return true;
}

bool AttackManager::Process()
{
	// フレームカウンタを増やす
	_frameCounter++;

	// 無効な攻撃のクリーンアップ
	CleanupInvalidAttacks();

	return true;
}

bool AttackManager::Render()
{
	return true;
}

// 攻撃の登録
void AttackManager::RegisterAttack(std::shared_ptr<AttackBase> attack, ATTACK_OWNER_TYPE ownerType, int ownerId)
{
	if(attack == nullptr){ return; }

	// 既に登録されているかチェック
	if(IsAttackRegistered(attack)) { return; }

	// 攻撃情報を作成して登録リストに追加
	ATTACK_INFO attackInfo;	// 攻撃情報構造体

	attackInfo.attack = attack;						// 攻撃オブジェクト
	attackInfo.ownerType = ownerType;				// 所有者タイプ
	attackInfo.ownerId = ownerId;					// 所有者ID
	attackInfo.registrationFrame = _frameCounter;	// 登録フレーム数
	attackInfo.isValid = true;						// 有効フラグ

	_registeredAttacks.push_back(attackInfo);		// リストに追加
}

// 攻撃の解除
void AttackManager::UnregisterAttack(std::shared_ptr<AttackBase> attack)
{
	if(attack == nullptr){ return; }

	// 登録リストから該当する攻撃を無効化
	for(auto& attackInfo : _registeredAttacks) // 登録攻撃リストを走査
	{
		// 攻撃オブジェクトを取得
		if(auto registeredAttack = attackInfo.attack.lock()) // weak_ptrをshared_ptrに変換
		{
			// 攻撃オブジェクトが一致するかチェック
			if(registeredAttack == attack)
			{
				attackInfo.isValid = false;	// 無効化
				break;
			}
		}
	}
}

// 所有者による攻撃解除
void AttackManager::UnregisterAttackByOwner(int ownerId)
{
	// 登録リストから該当する所有者IDの攻撃を無効化
	for(auto& attackInfo : _registeredAttacks) // 登録攻撃リストを走査
	{
		// 所有者IDが一致するかチェック
		if(attackInfo.ownerId == ownerId)
		{
			attackInfo.isValid = false;	// 無効化
		}
	}
}

// 全ての攻撃解除
void AttackManager::ClearAllAttacks()
{
	// 登録リストをクリア
	_registeredAttacks.clear();
}

// 無効な攻撃の削除
void AttackManager::CleanupInvalidAttacks()
{
	// 登録リストを走査
	auto registeredList = _registeredAttacks.begin();

	while(registeredList != _registeredAttacks.end())
	{
		// 攻撃オブジェクトが無効か、有効フラグがfalseなら削除
		if(registeredList->attack.expired() || !registeredList->isValid)
		{
			// 無効ならリストから削除
			registeredList = _registeredAttacks.erase(registeredList);
		}
		else
		{
			// 有効なら次へ
			++registeredList;
		}
	}
}

// 攻撃が登録されているかチェック
bool AttackManager::IsAttackRegistered(std::shared_ptr<AttackBase> attack)const
{
	if(attack == nullptr){ return false; }

	// 登録リストを走査
	for(auto& attackInfo : _registeredAttacks)
	{
		// 攻撃オブジェクトが一致するかチェック
		if(!attackInfo.attack.expired() && attackInfo.attack.lock() == attack)
		{
			return true;
		}
	}

	return false;
}

// 全てのアクティブな攻撃を取得
std::vector<std::shared_ptr<AttackBase>> AttackManager::GetAllActiveAttacks()const
{
	std::vector<std::shared_ptr<AttackBase>> activeAttacks;	// アクティブ攻撃リスト

	// 登録リストを走査
	for(auto& attackInfo : _registeredAttacks)
	{
		// 攻撃オブジェクトが有効かチェック
		if(!attackInfo.attack.expired())
		{
			auto attack = attackInfo.attack.lock();	// 攻撃オブジェクト取得

			// 攻撃状態がINACTIVEでないならアクティブ攻撃リストに追加
			if(attack->GetAttackState() != ATTACK_STATE::INACTIVE)
			{
				activeAttacks.push_back(attack); // リストに追加
			}
		}
	}

	return activeAttacks;
}

// 所有者タイプによる攻撃取得
std::vector<std::shared_ptr<AttackBase>> AttackManager::GetAttacksByOwnerType(ATTACK_OWNER_TYPE ownerType)const
{
	std::vector<std::shared_ptr<AttackBase>> ownerAttacks;	// 所有者タイプ攻撃リスト

	// 登録リストを走査
	for(auto& attackInfo : _registeredAttacks)
	{
		// 攻撃オブジェクトが有効かつ所有者タイプが一致するかチェック
		if(!attackInfo.attack.expired() && attackInfo.ownerType == ownerType)
		{
			auto attack = attackInfo.attack.lock();	// 攻撃オブジェクト取得

			// 攻撃状態がINACTIVEでないなら所有者タイプ攻撃リストに追加
			if(attack->GetAttackState() != ATTACK_STATE::INACTIVE)
			{
				ownerAttacks.push_back(attack);	// リストに追加
			}
		}
	}

	return ownerAttacks;
}

// 所有者IDによる攻撃取得
std::vector<std::shared_ptr<AttackBase>> AttackManager::GetAttacksByOwner(int ownerId) const
{
	std::vector<std::shared_ptr<AttackBase>> ownerAttacks;	// 所有者ID攻撃リスト

	// 登録リストを走査
	for(const auto& info : _registeredAttacks)
	{
		// 攻撃オブジェクトが有効かつ所有者IDが一致するかチェック
		if(!info.attack.expired() && info.ownerId == ownerId)
		{
			auto attack = info.attack.lock();	// 攻撃オブジェクト取得

			// 攻撃状態がINACTIVEでないなら所有者ID攻撃リストに追加
			if(attack->GetAttackState() != ATTACK_STATE::INACTIVE)
			{
				ownerAttacks.push_back(attack);	// リストに追加
			}
		}
	}

	return ownerAttacks;
}

// アクティブ攻撃数取得
int AttackManager::GetActiveAttackCount() const
{
	int count = 0;	// アクティブ攻撃数カウンタ

	// 登録リストを走査
	for(const auto& attackInfo : _registeredAttacks)
	{
		// 攻撃オブジェクトが有効かチェック
		if(!attackInfo.attack.expired())
		{
			auto attack = attackInfo.attack.lock();	// 攻撃オブジェクト取得

			// 攻撃状態がINACTIVEでないならカウンタを増やす
			if(attack->GetAttackState() != ATTACK_STATE::INACTIVE)
			{
				count++;
			}
		}
	}

	return count;
}

// インスタンス作成
void AttackManager::CreateInstance()
{
	if(_instance == nullptr)
	{
		_instance = new AttackManager();	// インスタンス作成
		_instance->Initialize();			// 初期化
	}
}

// インスタンス破棄
void AttackManager::DestroyInstance()
{
	if(_instance != nullptr)
	{
		_instance->Terminate();	// 終了

		delete _instance;		// インスタンス破棄
	}
}