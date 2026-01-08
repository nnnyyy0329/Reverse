// 担当 : 成田

#include "SurfacePlayer.h"

// 攻撃コリジョンの設定
void SurfacePlayer::InitializeAttackData()
{
	// 第1攻撃：カプセル攻撃
	_firstAttack.SetCapsuleAttackData
	(
		VGet(0.0f, 100.0f, 50.0f),	// 上部
		VGet(0.0f, 50.0f, 50.0f),	// 下部
		30.0f,						// 半径
		10.0f,						// 発生フレーム
		15.0f,						// 持続フレーム
		20.0f,						// 硬直フレーム
		5.0f,						// ダメージ
		false						// ヒットフラグ
	);

	// 第2攻撃：カプセル攻撃
	_secondAttack.SetCapsuleAttackData
	(
		VGet(0.0f, 250.0f, 50.0f),	// 上部
		VGet(0.0f, 150.0f, 50.0f),	// 下部
		30.0f,						// 半径
		10.0f,						// 発生フレーム
		15.0f,						// 持続フレーム
		20.0f,						// 硬直フレーム
		5.0f,						// ダメージ
		false						// ヒットフラグ
	);

	// 第3攻撃：カプセル攻撃
	_thirdAttack.SetCapsuleAttackData
	(
		VGet(0.0f, 200.0f, 50.0f),	// 上部
		VGet(0.0f, 0.0f, 50.0f),	// 下部
		30.0f,						// 半径
		10.0f,						// 発生フレーム
		15.0f,						// 持続フレーム
		20.0f,						// 硬直フレーム
		20.0f,						// ダメージ
		false						// ヒットフラグ
	);
}

// コリジョン位置の更新関数
void SurfacePlayer::ProcessAttackColPos()
{
	float offsetScale = 75.0f;	// ずらす大きさ

	// 判定を前方にずらす
	VECTOR dirNorm = VNorm(_vDir);						// 正規化
	VECTOR attackOffset = VScale(dirNorm, offsetScale);	// スケールに大きさ分足す
	VECTOR colOffset = VAdd(_vPos, attackOffset);		// ずらす

	// 第1攻撃コリジョン位置を更新
	_firstAttack.SetCapsuleAttackData
	(
		VAdd(colOffset, VGet(0.0f, 100.0f, 50.0f)),	// 上部
		VAdd(colOffset, VGet(0.0f, 50.0f, 50.0f)),	// 下部
		30.0f,										// 半径
		10.0f,										// 発生フレーム
		15.0f,										// 持続フレーム
		20.0f,										// 硬直フレーム
		10.0f,										// ダメージ
		false										// ヒットフラグ
	);

	// 第2攻撃コリジョン位置を更新
	_secondAttack.SetCapsuleAttackData
	(
		VAdd(colOffset, VGet(0.0f, 250.0f, 50.0f)),	// 上部
		VAdd(colOffset, VGet(0.0f, 150.0f, 50.0f)),	// 下部
		30.0f,										// 半径
		10.0f,										// 発生フレーム
		15.0f,										// 持続フレーム
		20.0f,										// 硬直フレーム
		10.0f,										// ダメージ
		false										// ヒットフラグ
	);

	// 第3攻撃コリジョン位置を更新
	_thirdAttack.SetCapsuleAttackData
	(
		VAdd(colOffset, VGet(0.0f, 200.0f, 50.0f)),	// 上部
		VAdd(colOffset, VGet(0.0f, 0.0f, 50.0f)),	// 下部
		30.0f,										// 半径
		10.0f,										// 発生フレーム
		15.0f,										// 持続フレーム
		20.0f,										// 硬直フレーム
		10.0f,										// ダメージ
		false										// ヒットフラグ
	);
}

// 攻撃Process呼び出し用関数
void SurfacePlayer::CallProcessAttack()
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
void SurfacePlayer::ProcessAttack()
{
	// 攻撃入力チェック（通常状態時）
	if((_ePlayerStatus == PLAYER_STATUS::WAIT ||
		_ePlayerStatus == PLAYER_STATUS::WALK) &&
		_trg & PAD_INPUT_7)
	{
		// 攻撃開始時にコリジョン位置を更新
		ProcessAttackColPos();

		// 攻撃コリジョンの情報を受け取る
		ReceiveAttackColData();

		_ePlayerStatus = PLAYER_STATUS::FIRST_ATTACK;
		_firstAttack.ProcessStartAttack();
		_iComboCount = 1;
		_bCanCombo = false;
	}
}

// 攻撃分岐処理
void SurfacePlayer::ProcessBranchAttack()
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

// 1段目の攻撃
void SurfacePlayer::ProcessFirstAttack()
{
	// 第1攻撃の状態更新
	ATTACK_STATE state = _firstAttack.GetAttackState();

	switch(state)
	{
		case ATTACK_STATE::STARTUP:
		{
			// コンボ受付なし
			_bCanCombo = false;
			break;
		}

		case ATTACK_STATE::ACTIVE:
		{
			// コンボ受付開始
			_bCanCombo = true;

			// 2段目の入力チェック
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				// 次の攻撃開始時にもコリジョン位置を更新
				ProcessAttackColPos();

				// 攻撃コリジョンの情報を受け取る
				ReceiveAttackColData();

				SetStatus(PLAYER_STATUS::SECOND_ATTACK);
				_secondAttack.ProcessStartAttack();
				_iComboCount = 2;
				_bCanCombo = false;
			}
			break;
		}

		case ATTACK_STATE::RECOVERY:
		{
			// 硬直中
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				// 次の攻撃開始時にもコリジョン位置を更新
				ProcessAttackColPos();

				// 攻撃コリジョンの情報を受け取る
				ReceiveAttackColData();

				SetStatus(PLAYER_STATUS::SECOND_ATTACK);
				_secondAttack.ProcessStartAttack();
				_iComboCount = 2;
				_bCanCombo = false;
			}
			break;
		}

		case ATTACK_STATE::INACTIVE:
		{
			// 攻撃完全終了
			SetStatus(PLAYER_STATUS::WAIT);
			_iComboCount = 0;
			_bCanCombo = false;
			break;
		}
	}
}

// 2段目の攻撃
void SurfacePlayer::ProcessSecondAttack()
{
	// 第2攻撃の状態更新
	ATTACK_STATE state = _secondAttack.GetAttackState();

	switch(state)
	{
		case ATTACK_STATE::STARTUP:
		{
			// コンボ受付なし
			_bCanCombo = false;
			break;
		}

		case ATTACK_STATE::ACTIVE:
		{
			// コンボ受付開始
			_bCanCombo = true;

			// 3段目入力チェック
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				// 次の攻撃開始時にもコリジョン位置を更新
				ProcessAttackColPos();

				// 攻撃コリジョンの情報を受け取る
				ReceiveAttackColData();

				SetStatus(PLAYER_STATUS::THIRD_ATTACK);
				_thirdAttack.ProcessStartAttack();
				_iComboCount = 3;
				_bCanCombo = false;
			}
			break;
		}

		case ATTACK_STATE::RECOVERY:
		{
			// 硬直中
			if((_trg & PAD_INPUT_7) && CanNextAttack())
			{
				// 次の攻撃開始時にもコリジョン位置を更新
				ProcessAttackColPos();

				// 攻撃コリジョンの情報を受け取る
				ReceiveAttackColData();

				SetStatus(PLAYER_STATUS::THIRD_ATTACK);
				_thirdAttack.ProcessStartAttack();
				_iComboCount = 3;
				_bCanCombo = false;
			}
			break;
		}

		case ATTACK_STATE::INACTIVE:
		{
			// 攻撃完全終了
			SetStatus(PLAYER_STATUS::WAIT);
			_iComboCount = 0;
			_bCanCombo = false;
			break;
		}
	}
}

// 3段目の攻撃
void SurfacePlayer::ProcessThirdAttack()
{
	// 第3攻撃の状態更新
	ATTACK_STATE state = _thirdAttack.GetAttackState();

	if(state == ATTACK_STATE::INACTIVE)
	{
		// 攻撃終了
		SetStatus(PLAYER_STATUS::WAIT);
		_iComboCount = 0;
		_bCanCombo = false;
	}
}

// 攻撃コリジョンの情報受け取り用関数
void SurfacePlayer::ReceiveAttackColData()
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
	_vAttackColTop		= attackCol.attackColTop;
	_vAttackColBottom	= attackCol.attackColBottom;
	_fAttackColR		= attackCol.attackColR;
}

// 次の攻撃が可能かチェック
bool SurfacePlayer::CanNextAttack()
{
	// コンボが可能で、最大コンボ数以下の場合のみ次の攻撃可能
	if(_bCanCombo && _iComboCount < 3)
	{
		return true;
	}

	return false;
}

// 攻撃状態中かどうかをチェック
bool SurfacePlayer::IsAttacking()
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