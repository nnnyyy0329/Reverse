// 担当 : 成田

#include "AttackManager.h"

// シングルトン用メンバ初期化
AttackManager* AttackManager::_instance = nullptr;

AttackManager::AttackManager()
{
	_frameCounter = 0;			// フレームカウンタ初期化
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

	// 登録されている攻撃を更新
	for (auto& attackInfo : _registeredAttacks)
	{
		if(attackInfo.attack.expired()){ continue; }	// 無効な攻撃はスキップ
		auto attack = attackInfo.attack.lock();			// 攻撃情報の取得

		if(attack == nullptr){ continue; }	// 無効な攻撃はスキップ
		attack->Process();					// 攻撃更新処理
	}

	// 無効な攻撃のクリーンアップ
	CleanupInvalidAttacks();

	return true;
}

void AttackManager::DebugRender()
{
	int registeredCount = GetRegisteredAttackCount();	// 登録攻撃数取得
	int activeCount = GetActiveAttackCount();			// アクティブ攻撃数取得

	DrawFormatString(10, 210, GetColor(0, 255, 0), "Registered Attacks: %d", registeredCount);
	DrawFormatString(10, 230, GetColor(0, 255, 0), "Active Attacks: %d", activeCount);
}

void AttackManager::CollisionRender()
{
	// 登録されている攻撃を描画
	for (auto& attackInfo : _registeredAttacks)
	{
		if (attackInfo.attack.expired()) { continue; }	// 無効な攻撃はスキップ
		auto attack = attackInfo.attack.lock();			// 攻撃情報の取得

		if (attack == nullptr) { continue; }	// 無効な攻撃はスキップ
		attack->Render();					// 攻撃描画処理
	}
}

// 攻撃の登録
void AttackManager::RegisterAttack(std::shared_ptr<AttackBase> attack, ATTACK_OWNER_TYPE ownerType, int ownerId)
{
	if(attack == nullptr){ return; }

	// 既に登録されているかチェック
	if(IsAttackRegistered(attack)) { return; }	// 登録済みなら何もしない

	// 攻撃情報を作成して登録リストに追加
	ATTACK_INFO attackInfo;	// 攻撃情報構造体

	attackInfo.attack = attack;						// 攻撃オブジェクト
	attackInfo.ownerType = ownerType;				// 所有者タイプ
	attackInfo.ownerId = ownerId;					// 所有者ID
	attackInfo.registrationFrame = _frameCounter;	// 登録フレーム数

	_registeredAttacks.push_back(attackInfo);		// リストに追加
}

// 攻撃の解除
void AttackManager::UnregisterAttack(std::shared_ptr<AttackBase> attack)
{
	if(attack == nullptr){ return; }

	auto attackPtr = attack;

	// 通常のforループで削除対象を探す
	for(auto attacks = _registeredAttacks.begin(); attacks != _registeredAttacks.end();)
	{
		// weak_ptrが無効になっているかチェック
		if(attacks->attack.expired())
		{
			// 無効な攻撃は削除
			attacks = _registeredAttacks.erase(attacks);

			continue;	// 次の要素へ
		}

		// 攻撃オブジェクトが一致するかチェック
		auto registeredAttack = attacks->attack.lock();

		// 一致するかチェック
		if(registeredAttack == attackPtr)
		{
			// 一致した攻撃を削除
			attacks = _registeredAttacks.erase(attacks);

			break;	// 見つかったので終了
		}
		else
		{
			// 次の要素へ
			++attacks;
		}
	}
}

// 所有者による攻撃解除
void AttackManager::UnregisterAttackByOwner(int ownerId)
{
	// 登録リストから該当する所有者IDの攻撃を無効化
	for(auto attacks = _registeredAttacks.begin(); attacks != _registeredAttacks.end();)
	{
		// 所有者IDが一致するかチェック
		if(attacks->ownerId == ownerId)
		{
			// 一致した攻撃を削除
			attacks = _registeredAttacks.erase(attacks);
		}
		else // 一致しないなら
		{
			// 次の要素へ
			++attacks;
		}
	}
}

// 全ての攻撃解除
void AttackManager::ClearAllAttacks()
{
	// 登録リストをクリア
	_registeredAttacks.clear();
}

// 無効な攻撃の削除//
void AttackManager::CleanupInvalidAttacks()
{
	// 通常のforループで無効な攻撃を削除
	for(auto attacks = _registeredAttacks.begin(); attacks != _registeredAttacks.end();)
	{
		// 削除フラグ
		bool shouldRemove = false;	

		// weak_ptrが無効になっているかチェック
		if(attacks->attack.expired())
		{
			shouldRemove = true;
		}
		else // 有効な場合
		{
			// 攻撃状態をチェック
			auto attack = attacks->attack.lock();

			// 攻撃状態がINACTIVEなら削除フラグを立てる
			if(attack->GetAttackState() == ATTACK_STATE::INACTIVE)
			{
				shouldRemove = true;
			}
		}

		// 削除が必要な場合
		if(shouldRemove)
		{
			// 攻撃を削除
			attacks = _registeredAttacks.erase(attacks);
		}
		else // 削除が不要な場合
		{
			// 次の要素へ
			++attacks;
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

// 回避にヒットした攻撃を登録
void AttackManager::RegisterDodgeHitAttack(std::shared_ptr<AttackBase> attack)
{
	if(attack == nullptr){ return; }

	// 登録済みなら何もしない
	if(IsDodgeHitAttack(attack)) { return; }	

	// 回避にヒットした攻撃リストに追加
	_dodgeHitAttacks.push_back(attack);
}

// 回避にヒットした攻撃かチェック
bool AttackManager::IsDodgeHitAttack(std::shared_ptr<AttackBase> attack) const
{
	if(attack == nullptr){ return false; }

	// 回避にヒットした攻撃リストを走査
	for(auto& registeredAttack : _dodgeHitAttacks)
	{
		if(!registeredAttack.expired() && registeredAttack.lock() == attack)
		{
			return true;
		}
	}

	return false;
}

// 回避にヒットした攻撃をクリア
void AttackManager::ClearDodgeHitAttacks()
{
	// リストクリア
	_dodgeHitAttacks.clear();
}

// 回避にヒットした攻撃のクリーンアップ
bool AttackManager::CleanupDodgeHitAttacks()
{
	// 通常のforループで無効な攻撃を削除
	for(auto attacks = _dodgeHitAttacks.begin(); attacks != _dodgeHitAttacks.end();)
	{
		// 削除フラグ
		bool shouldRemove = false;	

		// weak_ptrが無効になっているかチェック
		if(attacks->expired())
		{
			shouldRemove = true;
		}
		else // 有効な場合
		{
			// 攻撃状態をチェック
			auto attack = attacks->lock();

			// 攻撃状態がINACTIVEなら削除フラグを立てる
			if(attack->GetAttackState() == ATTACK_STATE::INACTIVE)
			{
				shouldRemove = true;
			}
		}

		// 削除が必要な場合
		if(shouldRemove)
		{
			// 攻撃を削除
			attacks = _dodgeHitAttacks.erase(attacks);
		}
		else // 削除が不要な場合
		{
			// 次の要素へ
			++attacks;
		}
	}
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

			// 攻撃状態がACTIVEならアクティブ攻撃リストに追加
			if(attack->GetAttackState() == ATTACK_STATE::ACTIVE)
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
	for(auto& info : _registeredAttacks)
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
	for(auto& attackInfo : _registeredAttacks)
	{
		// 攻撃オブジェクトが有効かチェック
		if(!attackInfo.attack.expired())
		{
			auto attack = attackInfo.attack.lock();	// 攻撃オブジェクト取得

			// 攻撃状態がACTIVEならカウンタを増やす
			if(attack->GetAttackState() == ATTACK_STATE::ACTIVE)
			{
				count++;
			}
		}
	}

	return count;
}

// 攻撃の所有者タイプを取得
ATTACK_OWNER_TYPE AttackManager::GetAttackOwnerType(std::shared_ptr<AttackBase> attack) const
{
	if(attack == nullptr) { return ATTACK_OWNER_TYPE::NONE; }

	// 登録リストから該当する攻撃を検索
	for(const auto& attackInfo : _registeredAttacks)
	{
		// 攻撃情報が有効かチェック
		if(!attackInfo.attack.expired() && attackInfo.attack.lock() == attack)
		{
			// 攻撃の所有者タイプを返す
			return attackInfo.ownerType;
		}
	}

	return ATTACK_OWNER_TYPE::NONE;
}

// 攻撃の所有者IDを取得
int AttackManager::GetAttackOwnerId(std::shared_ptr<AttackBase> attack) const
{
	if(attack == nullptr) { return -1; }

	// 登録リストから該当する攻撃を検索
	for(const auto& attackInfo : _registeredAttacks)
	{
		// 攻撃情報が有効かチェック
		if(!attackInfo.attack.expired() && attackInfo.attack.lock() == attack)
		{
			// 攻撃の所有者IDを返す
			return attackInfo.ownerId;
		}
	}

	return -1;
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