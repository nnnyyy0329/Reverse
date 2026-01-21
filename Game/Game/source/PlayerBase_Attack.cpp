// 担当 : 成田

#include "PlayerBase.h"
#include "AttackManager.h"

namespace
{
	const int INTERIOR_PLAYER_ID = 1;	// 裏プレイヤーID
	const int SURFACE_PLAYER_ID = 2;	// 表プレイヤーID
}

// 攻撃コリジョンの設定
void PlayerBase::InitializeAttackData()
{
	// 子クラスから攻撃定数と設定を取得
	AttackConstants constants = GetAttackConstants();	// 攻撃定数取得

	// キャラタイプに応じた最大コンボ数を取得
	int maxComboCount;
	if(_eCharaType == CHARA_TYPE::SURFACE_PLAYER)
	{
		maxComboCount = constants.SURFACE_MAX_COMBO_COUNT;
	}
	else
	{
		maxComboCount = constants.INTERIOR_MAX_COMBO_COUNT;
	}

	// 攻撃設定取得
	std::vector<AttackConfig>configs(maxComboCount);	
	GetAttackConfigs(configs.data());

	// 攻撃配列とステータス配列を初期化
	_attacks.clear();
	_attackStatuses.clear();

	// 攻撃状態の定義
	std::vector<PLAYER_STATUS> statuses =
	{
		PLAYER_STATUS::FIRST_ATTACK,
		PLAYER_STATUS::SECOND_ATTACK,
		PLAYER_STATUS::THIRD_ATTACK,
		PLAYER_STATUS::FOURTH_ATTACK,
		PLAYER_STATUS::FIFTH_ATTACK
	};

	// 動的に攻撃データを作成
	for(int i = 0; i < maxComboCount; ++i)
	{
		auto attack = std::make_shared<AttackBase>();

		// 攻撃配列に追加
		attack->SetCapsuleAttackData
		(
			configs[i].topOffset,       // 上部
			configs[i].bottomOffset,    // 下部
			constants.COMMON_RADIUS,    // 半径
			constants.COMMON_DELAY,     // 発生フレーム
			constants.COMMON_DURATION,  // 持続フレーム
			constants.COMMON_RECOVERY,  // 硬直フレーム
			configs[i].damage,          // ダメージ
			false                       // ヒットフラグ
		);

		_attacks.push_back(attack);
		_attackStatuses.push_back(statuses[i]);
	}
}

// 攻撃のコリジョン位置更新
void PlayerBase::UpdateAttackColPos
(
	std::shared_ptr<AttackBase> attack,
	VECTOR& topOffset,
	VECTOR& bottomOffset, 
	VECTOR& baseOffset
)
{
	if(!attack) return;

	// 攻撃コリジョン情報を取得
	const ATTACK_COLLISION& col = attack->GetAttackCollision();

	// コリジョン位置を更新
	attack->SetCapsuleAttackData
	(
		VAdd(baseOffset, topOffset),	// 上部
		VAdd(baseOffset, bottomOffset),	// 下部
		col.attackColR,					// 半径
		col.attackDelay,				// 発生遅延
		col.attackDuration,				// 持続時間
		col.recovery, 					// 後隙
		col.damage, 					// ダメージ
		col.isHit						// ヒットフラグ
	);
}

// コリジョン位置の更新関数
void PlayerBase::ProcessAttackColPos()
{
	// 子クラスから攻撃定数と設定を取得
	AttackConstants constants = GetAttackConstants();	// 攻撃定数取得
	std::vector<AttackConfig> configs(_attacks.size());	// 攻撃設定取得
	GetAttackConfigs(configs.data());					// 攻撃設定取得

	// 判定を前方にずらす
	VECTOR dirNorm = VNorm(_vDir);
	VECTOR attackOffset = VScale(dirNorm, constants.ATTACK_OFFSET_SCALE);
	VECTOR colOffset = VAdd(_vPos, attackOffset);

	// 攻撃配列から各攻撃のコリジョン位置を更新
	for(size_t i = 0; i < _attacks.size(); ++i)
	{
		UpdateAttackColPos(_attacks[i], configs[i].topOffset, configs[i].bottomOffset, colOffset);
	}
}

// 攻撃Process呼び出し用関数
void PlayerBase::CallProcessAttack()
{
	// 攻撃処理
	ProcessAttack();

	// 攻撃分岐処理
	ProcessBranchAttack();
}

// 攻撃処理
void PlayerBase::ProcessAttack()
{
	// 攻撃入力チェック（通常状態時）
	if((_ePlayerStatus == PLAYER_STATUS::WAIT ||
		_ePlayerStatus == PLAYER_STATUS::WALK)
		&& _trg & PAD_INPUT_7) 
	{
		// 1段目の攻撃開始処理
		ProcessStartAttack(1, PLAYER_STATUS::FIRST_ATTACK, _attacks[0]);
	}
}

// コンボ攻撃開始の処理
void PlayerBase::ProcessStartAttack(int comboCount, PLAYER_STATUS nextStatus, std::shared_ptr<AttackBase> attack)
{
	ProcessAttackColPos();							// コリジョン位置更新処理
	ReceiveAttackColData();							// 攻撃判定受け取り関数
	SetStatus(nextStatus);							// 状態更新
	attack->ProcessStartAttack();					// 攻撃処理開始

	// AttackManagerに登録
	std::shared_ptr<AttackBase> attackPtr = attack;
	if(attackPtr != nullptr)
	{
		// プレイヤーごとに情報を変えて登録する
		switch(_eCharaType)
		{
			case CHARA_TYPE::SURFACE_PLAYER:
			{
				AttackManager::GetInstance()->RegisterAttack(attackPtr, ATTACK_OWNER_TYPE::SURFACE_PLAYER, GetInstanceId());
				break;
			}

			case CHARA_TYPE::INTERIOR_PLAYER:
			{
				AttackManager::GetInstance()->RegisterAttack(attackPtr, ATTACK_OWNER_TYPE::INTERIOR_PLAYER, GetInstanceId());
				break;
			}
		}
	}

	_iComboCount = comboCount;
	_bCanCombo = false;
}

// 攻撃分岐処理
void PlayerBase::ProcessBranchAttack()
{
	// 現在の状態に応じて攻撃処理を分岐
	int currentAttackIndex = GetAttackIndexByStatus(_ePlayerStatus);

	if(currentAttackIndex >= 0 && currentAttackIndex < static_cast<int>(_attacks.size()))
	{
		// 汎用コンボ処理
		ProcessComboAttack(currentAttackIndex);
	}
}

// 汎用コンボ攻撃処理
void PlayerBase::ProcessComboAttack(int attackIndex)
{
	auto currentAttack = _attacks[attackIndex];
	ATTACK_STATE state = currentAttack->GetAttackState();

	// 状態に応じた処理
	switch(state)
	{
		case ATTACK_STATE::STARTUP:	// 発生前
		{
			// コンボ不可にする
			_bCanCombo = false;	
			break;
		}

		case ATTACK_STATE::ACTIVE:	// 攻撃判定中
		{
			// コンボ可能にする
			_bCanCombo = true;

			// 次の攻撃入力があれば次の攻撃へ
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				ProcessNextAttack(attackIndex);
			}
			break;
		}

		case ATTACK_STATE::RECOVERY:	// 硬直中
		{
			// 次の攻撃入力があれば次の攻撃へ
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				ProcessNextAttack(attackIndex);
			}
			break;
		}

		case ATTACK_STATE::INACTIVE:	// 攻撃終了
		{
			// 攻撃過程終了処理
			EndAttackSequence();
			break;
		}
	}
}

// 攻撃終了処理
void PlayerBase::ProcessAttackFinish(std::shared_ptr<AttackBase> attack)
{
	// 攻撃状態が非アクティブなら攻撃過程終了
	if(attack->GetAttackState() == ATTACK_STATE::INACTIVE)
	{
		// 攻撃過程終了処理
		EndAttackSequence();
	}
}

// 攻撃過程終了
void PlayerBase::EndAttackSequence()
{
	// AttackManagerから自分の攻撃を全て解除
	AttackManager::GetInstance()->UnregisterAttackByOwner(GetInstanceId());

	SetStatus(PLAYER_STATUS::WAIT);	// 状態を待機に戻す
	_iComboCount = 0;				// コンボカウントリセット
	_bCanCombo = false;				// コンボ不可にする
}

// 次の攻撃へ
void PlayerBase::ProcessNextAttack(int currentIndex)
{
	// 次の攻撃インデックスを計算
	int nextIndex = currentIndex + 1;

	// 次の攻撃が存在する場合
	if(nextIndex < static_cast<int>(_attacks.size()))
	{
		PLAYER_STATUS nextStatus = _attackStatuses[nextIndex];								// 次の状態取得
		ProcessStartAttack(nextIndex + 1, _attackStatuses[nextIndex], _attacks[nextIndex]);	// 次の攻撃へ
	}
}

// 攻撃コリジョンの情報受け取り用関数
void PlayerBase::ReceiveAttackColData()
{
	// 現在の攻撃インデックスを取得
	int attackIndex = GetAttackIndexByStatus(_ePlayerStatus);

	if(attackIndex >= 0 && attackIndex < static_cast<int>(_attacks.size()))
	{
		// 攻撃コリジョン情報を取得
		ATTACK_COLLISION attackCol = _attacks[attackIndex]->GetAttackCollision();

		// コリジョン情報を入れる
		_vAttackColTop = attackCol.attackColTop;
		_vAttackColBottom = attackCol.attackColBottom;
		_fAttackColR = attackCol.attackColR;
	}
	else
	{
		// 攻撃状態でない場合は値をリセット
		_vAttackColTop = VGet(0.0f, 0.0f, 0.0f);
		_vAttackColBottom = VGet(0.0f, 0.0f, 0.0f);
		_fAttackColR = 0.0f;
	}
}

// 次の攻撃が可能かチェック
bool PlayerBase::CanNextAttack()
{
	// コンボが可能で、最大コンボ数以下の場合のみ次の攻撃可能
	AttackConstants constants = GetAttackConstants();

	// コンボ可能かつ最大コンボ数以下かチェック
	int maxComboCount;

	// キャラタイプに応じた最大コンボ数を取得
	if(_eCharaType == CHARA_TYPE::SURFACE_PLAYER)
	{
		maxComboCount = constants.SURFACE_MAX_COMBO_COUNT;
	}
	else
	{
		maxComboCount = constants.INTERIOR_MAX_COMBO_COUNT;
	}


	return _bCanCombo && _iComboCount < maxComboCount;
}

// 攻撃状態中かどうかをチェック
bool PlayerBase::IsAttacking()
{
	// 攻撃状態中かチェック
	if(_ePlayerStatus == PLAYER_STATUS::FIRST_ATTACK ||
		_ePlayerStatus == PLAYER_STATUS::SECOND_ATTACK ||
		_ePlayerStatus == PLAYER_STATUS::THIRD_ATTACK)
	{
		_vMove = VGet(0, 0, 0);	// 攻撃中は移動不可
		return true;
	}

	return false;
}

// ヘルパー関数
std::shared_ptr<AttackBase> PlayerBase::GetAttackByStatus(PLAYER_STATUS status)
{
	int index = GetAttackIndexByStatus(status);	// 攻撃インデックス取得

	// インデックスが有効範囲内かチェック
	if(index >= 0 && index < static_cast<int>(_attacks.size()))
	{
		// インデックスに対応する攻撃を返す
		return _attacks[index];
	}

	return nullptr;
}

// ID取得関数
int PlayerBase::GetInstanceId()
{
	// キャラタイプに応じたIDを返す
	switch(_eCharaType)
	{
		case CHARA_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			return INTERIOR_PLAYER_ID;	// 裏プレイヤーID
		}

		case CHARA_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			return SURFACE_PLAYER_ID;	// 表プレイヤーID
		}

		default:
		{
			return 0;
		}
	}
}

// 状態から攻撃インデックスを取得
int PlayerBase::GetAttackIndexByStatus(PLAYER_STATUS status)
{
	// 攻撃状態配列からインデックスを検索
	for(size_t i = 0; i < _attackStatuses.size(); ++i)
	{
		// 状態が一致したらインデックスを返す
		if(_attackStatuses[i] == status)
		{
			// 一致したインデックスを返す
			return static_cast<int>(i);
		}
	}

	return -1; // 見つからなかった場合
}
