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
	AttackConfig configs[3];							// 攻撃設定取得
	GetAttackConfigs(configs);							// 攻撃設定取得

	// 第1攻撃：カプセル攻撃
	_firstAttack = std::make_shared<AttackBase>();
	_firstAttack->SetCapsuleAttackData
	(
		configs[0].topOffset,		// 上部
		configs[0].bottomOffset,	// 下部
		constants.COMMON_RADIUS,	// 半径
		constants.COMMON_DELAY,		// 発生フレーム
		constants.COMMON_DURATION, 	// 持続フレーム
		constants.COMMON_RECOVERY,	// 硬直フレーム
		configs[0].damage,			// ダメージ
		false						// ヒットフラグ
	);

	// 第2攻撃：カプセル攻撃
	_secondAttack = std::make_shared<AttackBase>();
	_secondAttack->SetCapsuleAttackData
	(
		configs[1].topOffset,		// 上部
		configs[1].bottomOffset,	// 下部
		constants.COMMON_RADIUS,	// 半径
		constants.COMMON_DELAY, 	// 発生フレーム
		constants.COMMON_DURATION,	// 持続フレーム
		constants.COMMON_RECOVERY,	// 硬直フレーム
		configs[1].damage,			// ダメージ
		false						// ヒットフラグ
	);

	// 第3攻撃：カプセル攻撃
	_thirdAttack = std::make_shared<AttackBase>();
	_thirdAttack->SetCapsuleAttackData
	(
		configs[2].topOffset,		// 上部
		configs[2].bottomOffset,	// 下部
		constants.COMMON_RADIUS,	// 半径
		constants.COMMON_DELAY,		// 発生フレーム
		constants.COMMON_DURATION, 	// 持続フレーム
		constants.COMMON_RECOVERY,	// 硬直フレーム
		configs[2].damage,			// ダメージ
		false						// ヒットフラグ
	);
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
	AttackConfig configs[3];							// 攻撃設定取得
	GetAttackConfigs(configs);							// 攻撃設定取得

	// 判定を前方にずらす
	VECTOR dirNorm = VNorm(_vDir);
	VECTOR attackOffset = VScale(dirNorm, constants.ATTACK_OFFSET_SCALE);
	VECTOR colOffset = VAdd(_vPos, attackOffset);

	// 各攻撃のコリジョン位置を更新
	UpdateAttackColPos(_firstAttack, configs[0].topOffset, configs[0].bottomOffset, colOffset);
	UpdateAttackColPos(_secondAttack, configs[1].topOffset, configs[1].bottomOffset, colOffset);
	UpdateAttackColPos(_thirdAttack, configs[2].topOffset, configs[2].bottomOffset, colOffset);
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
		ProcessStartAttack(1, PLAYER_STATUS::FIRST_ATTACK, _firstAttack);
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
	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::FIRST_ATTACK:	// 1段目の攻撃
		{
			// 1段目の攻撃処理
			ProcessFirstAttack();

			break;
		}

		case PLAYER_STATUS::SECOND_ATTACK:	// 2段目の攻撃
		{
			// 2段目の攻撃処理
			ProcessSecondAttack();

			break;
		}

		case PLAYER_STATUS::THIRD_ATTACK:	// 3段目の攻撃
		{
			// 3段目の攻撃処理
			ProcessThirdAttack();

			break;
		}

		default:	// 攻撃状態でない場合
		{
			break;
		}
	}
}

// 汎用コンボ攻撃処理
void PlayerBase::ProcessComboAttack
(
	std::shared_ptr<AttackBase> currentAttack,	// 現在の攻撃
	int nextComboCount,							// 次のコンボカウント
	PLAYER_STATUS nextStatus,					// 次の状態
	std::shared_ptr<AttackBase> nextAttack		// 次の攻撃
)
{
	// 現在の攻撃状態を取得
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
				// 次の攻撃へ
				ProcessStartAttack(nextComboCount, nextStatus, nextAttack);
			}

			break;
		}

		case ATTACK_STATE::RECOVERY:	// 硬直中
		{
			// 次の攻撃入力があれば次の攻撃へ
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				// 次の攻撃へ
				ProcessStartAttack(nextComboCount, nextStatus, nextAttack);
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

// 1段目の攻撃
void PlayerBase::ProcessFirstAttack()
{
	// 汎用コンボ攻撃処理呼び出し
	ProcessComboAttack(_firstAttack, 2, PLAYER_STATUS::SECOND_ATTACK, _secondAttack);
}

// 2段目の攻撃
void PlayerBase::ProcessSecondAttack()
{
	// 汎用コンボ攻撃処理呼び出し
	ProcessComboAttack(_secondAttack, 3, PLAYER_STATUS::THIRD_ATTACK, _thirdAttack);
}

// 3段目の攻撃
void PlayerBase::ProcessThirdAttack()
{
	// 攻撃終了処理呼び出し
	ProcessAttackFinish(_thirdAttack);
}

// 攻撃コリジョンの情報受け取り用関数
void PlayerBase::ReceiveAttackColData()
{
	// 攻撃のコリジョン情報を取得
	ATTACK_COLLISION attackCol;

	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::FIRST_ATTACK:	// 1段目の攻撃
		{
			// 1段目の攻撃コリジョン情報取得
			attackCol = _firstAttack->GetAttackCollision();

			break;
		}

		case PLAYER_STATUS::SECOND_ATTACK:	// 2段目の攻撃
		{
			// 2段目の攻撃コリジョン情報取得
			attackCol = _secondAttack->GetAttackCollision();

			break;
		}

		case PLAYER_STATUS::THIRD_ATTACK:	// 3段目の攻撃
		{
			// 3段目の攻撃コリジョン情報取得
			attackCol = _thirdAttack->GetAttackCollision();

			break;
		}

		default:	// 攻撃状態でない場合
		{
			// 攻撃状態でない場合は値をリセット
			_vAttackColTop = VGet(0.0f, 0.0f, 0.0f);
			_vAttackColBottom = VGet(0.0f, 0.0f, 0.0f);
			_fAttackColR = 0.0f;
			return;
		}
	}

	// ProcessAttackColPos()で処理されたコリジョン情報を入れる
	_vAttackColTop = attackCol.attackColTop;
	_vAttackColBottom = attackCol.attackColBottom;
	_fAttackColR = attackCol.attackColR;
}

// 次の攻撃が可能かチェック
bool PlayerBase::CanNextAttack()
{
	// コンボが可能で、最大コンボ数以下の場合のみ次の攻撃可能
	AttackConstants constants = GetAttackConstants();

	// コンボ可能かつ最大コンボ数以下かチェック
	return _bCanCombo && _iComboCount < constants.MAX_COMBO_COUNT;
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
	switch(status)
	{
		case PLAYER_STATUS::FIRST_ATTACK: // 1段目の攻撃
		{
			return _firstAttack;
		}

		case PLAYER_STATUS::SECOND_ATTACK: // 2段目の攻撃
		{
			return _secondAttack;
		}

		case PLAYER_STATUS::THIRD_ATTACK: // 3段目の攻撃
		{
			return _thirdAttack;
		}

		default: // 攻撃状態でない場合
		{
			return nullptr;
		}
	}
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
