// 担当 : 成田

#include "PlayerBase.h"

// 攻撃コリジョンの設定
void PlayerBase::InitializeAttackData()
{
	// 子クラスから攻撃定数と設定を取得
	AttackConstants constants = GetAttackConstants();
	AttackConfig configs[3];
	GetAttackConfigs(configs);

	// 第1攻撃：カプセル攻撃
	_firstAttack.SetCapsuleAttackData
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
	_secondAttack.SetCapsuleAttackData
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
	_thirdAttack.SetCapsuleAttackData
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

// 単一攻撃のコリジョン位置更新
void PlayerBase::UpdateAttackColPos
(
	AttackBase& attack,
	const VECTOR& topOffset,
	const VECTOR& bottomOffset, 
	const VECTOR& baseOffset
)
{
	const ATTACK_COLLISION& col = attack.GetAttackCollision();

	attack.SetCapsuleAttackData
	(
		VAdd(baseOffset, topOffset),
		VAdd(baseOffset, bottomOffset),
		col.attackColR,
		col.attackDelay,
		col.attackDuration,
		col.recovery, 
		col.damage, 
		col.isHit
	);
}

// コリジョン位置の更新関数
void PlayerBase::ProcessAttackColPos()
{
	// 子クラスから攻撃定数と設定を取得
	AttackConstants constants = GetAttackConstants();
	AttackConfig configs[3];
	GetAttackConfigs(configs);

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

	// 攻撃状態更新
	_firstAttack.UpdateAttackState();
	_secondAttack.UpdateAttackState();
	_thirdAttack.UpdateAttackState();
}

// 攻撃処理
void PlayerBase::ProcessAttack()
{
	// 攻撃入力チェック（通常状態時）
	if((_ePlayerStatus == PLAYER_STATUS::WAIT ||
		_ePlayerStatus == PLAYER_STATUS::WALK)
		&& _trg & PAD_INPUT_7) 
	{
		ProcessStartAttack(1, PLAYER_STATUS::FIRST_ATTACK, _firstAttack);
	}
}

// コンボ攻撃開始の処理
void PlayerBase::ProcessStartAttack(int comboCount, PLAYER_STATUS nextStatus, AttackBase& attack)
{
	ProcessAttackColPos();			// コリジョン位置更新処理
	ReceiveAttackColData();			// 攻撃判定受け取り関数
	SetStatus(nextStatus);			// 状態更新
	attack.ProcessStartAttack();	// 攻撃処理かいし
	_iComboCount = comboCount;
	_bCanCombo = false;
}

// 攻撃分岐処理
void PlayerBase::ProcessBranchAttack()
{
	// 現在の状態に応じて攻撃処理を分岐
	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::FIRST_ATTACK:
		{
			ProcessFirstAttack();
			break;
		}

		case PLAYER_STATUS::SECOND_ATTACK:
		{
			ProcessSecondAttack();
			break;
		}

		case PLAYER_STATUS::THIRD_ATTACK:
		{
			ProcessThirdAttack();
			break;
		}

		default:
		{
			break;
		}
	}
}

// 汎用コンボ攻撃処理
void PlayerBase::ProcessComboAttack
(
	AttackBase& currentAttack,
	int nextComboCount,
	PLAYER_STATUS nextStatus,
	AttackBase& nextAttack
)
{
	ATTACK_STATE state = currentAttack.GetAttackState();

	switch(state)
	{
		case ATTACK_STATE::STARTUP:
		{
			_bCanCombo = false;
			break;
		}

		case ATTACK_STATE::ACTIVE:
		{
			_bCanCombo = true;
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				ProcessStartAttack(nextComboCount, nextStatus, nextAttack);
			}
			break;
		}

		case ATTACK_STATE::RECOVERY:
		{
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				ProcessStartAttack(nextComboCount, nextStatus, nextAttack);
			}
			break;
		}

		case ATTACK_STATE::INACTIVE:
		{
			EndAttackSequence();
			break;
		}
	}
}

// 攻撃終了処理
void PlayerBase::ProcessAttackFinish(AttackBase& attack)
{
	if(attack.GetAttackState() == ATTACK_STATE::INACTIVE)
	{
		EndAttackSequence();
	}
}

// 攻撃過程終了
void PlayerBase::EndAttackSequence()
{
	SetStatus(PLAYER_STATUS::WAIT);
	_iComboCount = 0;
	_bCanCombo = false;
}

// 1段目の攻撃
void PlayerBase::ProcessFirstAttack()
{
	ProcessComboAttack(_firstAttack, 2, PLAYER_STATUS::SECOND_ATTACK, _secondAttack);
}

// 2段目の攻撃
void PlayerBase::ProcessSecondAttack()
{
	ProcessComboAttack(_secondAttack, 3, PLAYER_STATUS::THIRD_ATTACK, _thirdAttack);
}

// 3段目の攻撃
void PlayerBase::ProcessThirdAttack()
{
	ProcessAttackFinish(_thirdAttack);
}

// 攻撃コリジョンの情報受け取り用関数
void PlayerBase::ReceiveAttackColData()
{
	// 攻撃のコリジョン情報を取得
	ATTACK_COLLISION attackCol;

	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::FIRST_ATTACK:
		{
			attackCol = _firstAttack.GetAttackCollision();
			break;
		}

		case PLAYER_STATUS::SECOND_ATTACK:
		{
			attackCol = _secondAttack.GetAttackCollision();
			break;
		}

		case PLAYER_STATUS::THIRD_ATTACK:
		{
			attackCol = _thirdAttack.GetAttackCollision();
			break;
		}

		default:
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

	return _bCanCombo && _iComboCount < constants.MAX_COMBO_COUNT;
}

// 攻撃状態中かどうかをチェック
bool PlayerBase::IsAttacking()
{
	if(_ePlayerStatus == PLAYER_STATUS::FIRST_ATTACK ||
		_ePlayerStatus == PLAYER_STATUS::SECOND_ATTACK ||
		_ePlayerStatus == PLAYER_STATUS::THIRD_ATTACK)
	{
		_vMove = VGet(0, 0, 0);
		return true;
	}

	return false;
}
