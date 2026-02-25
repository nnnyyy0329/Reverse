// 担当 : 成田

#include "PlayerBase.h"
#include "AttackManager.h"

// プレイヤーID定数
namespace IdConstants
{
	constexpr int INTERIOR_PLAYER_ID = 1;	// 裏プレイヤーID
	constexpr int SURFACE_PLAYER_ID = 2;	// 表プレイヤーID
}

// 攻撃コリジョンの設定
void PlayerBase::InitializeAttackData()
{
	// 子クラスから攻撃定数と設定を取得
	AttackConstants constants = GetAttackConstants();	// 攻撃定数取得

	// キャラタイプに応じた最大コンボ数を取得
	int maxComboCount = GetMaxComboCount();

	// コンボカウントが0ならスキップ
	if(maxComboCount <= 0)
	{
		_attacks.clear();
		_attackStatuses.clear();
		return;
	}

	// 攻撃設定取得
	std::vector<AttackConfig>configs(maxComboCount);	
	GetAttackConfigs(configs.data());

	// 攻撃配列とステータス配列を初期化
	_attacks.clear();
	_attackStatuses.clear();

	// 攻撃状態の定義
	std::vector<PLAYER_ATTACK_STATE> statuses =
	{
		PLAYER_ATTACK_STATE::FIRST_ATTACK,
		PLAYER_ATTACK_STATE::SECOND_ATTACK,
		PLAYER_ATTACK_STATE::THIRD_ATTACK,
		PLAYER_ATTACK_STATE::FOURTH_ATTACK,
		PLAYER_ATTACK_STATE::FIFTH_ATTACK
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
			configs[i].radius,			// 半径
			_vMove,						// 攻撃方向
			configs[i].delay,           // 発生フレーム
			configs[i].duration,        // 持続フレーム
			configs[i].recovery,        // 硬直フレーム
			configs[i].damage,          // ダメージ
			false,						// ヒットフラグ
			configs[i].attackState,		// 攻撃状態
			configs[i].attackMoveSpeed,	// 攻撃中の移動速度
			configs[i].canKnockback		// 吹き飛ばし攻撃かどうか
		);

		_attacks.push_back(attack);
		_attackStatuses.push_back(statuses[i]);
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
		col.attackDir,					// 攻撃方向
		col.attackDelay,				// 発生遅延
		col.attackDuration,				// 持続時間
		col.recovery, 					// 後隙
		col.damage, 					// ダメージ
		col.isHit,						// ヒットフラグ
		col.attackState,				// 攻撃状態
		col.attackMoveSpeed,			// 攻撃中の移動速度
		col.canKnockback				// 吹き飛ばし攻撃かどうか
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
	VECTOR attackOffset = VScale(dirNorm, constants.attackOffsetScale);
	VECTOR colOffset = VAdd(_vPos, attackOffset);

	// 攻撃配列から各攻撃のコリジョン位置を更新
	for(size_t i = 0; i < _attacks.size(); ++i)
	{
		UpdateAttackColPos(_attacks[i], configs[i].topOffset, configs[i].bottomOffset, colOffset);
	}
}

// 攻撃処理
void PlayerBase::ProcessAttack()
{
	// 攻撃開始チェック
	if(IsStartAttack())
	{
		// 攻撃配列が空の場合は処理をスキップ
		if(_attacks.empty()){ return; }

		// 1段目の攻撃開始処理
		ProcessStartAttack(1, PLAYER_ATTACK_STATE::FIRST_ATTACK, _attacks[0]);
	}
}

// コンボ攻撃開始の処理
void PlayerBase::ProcessStartAttack(int comboCount, PLAYER_ATTACK_STATE nextStatus, std::shared_ptr<AttackBase> attack)
{
	// 攻撃オブジェクト設定
	attack->SetOwner(shared_from_this());

	// コリジョン位置更新処理
	ProcessAttackColPos();	

	// 攻撃判定受け取り関数
	ReceiveAttackColData();		

	// 状態更新
	_playerState.attackState = nextStatus;

	// 攻撃処理開始
	attack->ProcessStartAttack();		

	// 攻撃エフェクト処理
	// -1しているのは攻撃開始処理内で攻撃状態が更新されるため、次の攻撃状態を渡すため
	ProcessAttackReaction(comboCount - 1);	// 攻撃反応処理

	// AttackManagerに登録
	std::shared_ptr<AttackBase> attackPtr = attack;
	if(attackPtr != nullptr)
	{
		// プレイヤーごとに情報を変えて登録する
		switch(_eCharaType)
		{
			case CHARA_TYPE::SURFACE_PLAYER:
			{
				// 表プレイヤーとして登録
				AttackManager::GetInstance()->RegisterAttack(attackPtr, ATTACK_OWNER_TYPE::SURFACE_PLAYER, GetInstanceId());
				break;
			}

			case CHARA_TYPE::INTERIOR_PLAYER:
			{
				// 裏プレイヤーとして登録
				AttackManager::GetInstance()->RegisterAttack(attackPtr, ATTACK_OWNER_TYPE::INTERIOR_PLAYER, GetInstanceId());
				break;
			}
		}
	}

	_iComboCount = comboCount;	// コンボカウント設定
	_bCanCombo = false;			// コンボフラグ初期化
}

// 攻撃の反応処理
void PlayerBase::ProcessAttackReaction(int attackIndex)
{
	// キャラタイプに応じた最大コンボ数を取得
	int maxComboCount = GetMaxComboCount();

	// 攻撃設定取得
	std::vector<AttackConfig> configs(maxComboCount);
	GetAttackConfigs(configs.data());

	// 有効な攻撃インデックスかチェック
	if(attackIndex >= 0 && attackIndex < static_cast<int>(configs.size()))
	{
		// 攻撃エフェクト処理
		ProcessAttackEffect(attackIndex, configs);

		// 攻撃サウンド処理
		ProcessAttackSound(attackIndex, configs);
	}
}

// 攻撃エフェクト処理
void PlayerBase::ProcessAttackEffect(int attackIndex, std::vector<AttackConfig> configs)
{
	// 攻撃設定からエフェクト名とオフセットを取得
	AttackConfig& config = configs[attackIndex];

	// エフェクト名が空でない場合のみ
	if(!config.effectName.empty())
	{
		// エフェクト再生位置を計算
		VECTOR effectPos = VAdd(_vPos, config.effectOffset);

		// 攻撃方向に応じてオフセットを回転
		VECTOR dirNorm = VNorm(_vDir);
		VECTOR rotatedOffset = VGet
		(
			config.effectOffset.x * dirNorm.x - config.effectOffset.z * dirNorm.z,	// X成分
			config.effectOffset.y,													// Y成分
			config.effectOffset.x * dirNorm.z + config.effectOffset.z * dirNorm.x	// Z成分
		);
		effectPos = VAdd(_vPos, rotatedOffset);

		// エフェクト再生
		EffectServer::GetInstance()->Play(config.effectName, effectPos);
	}
}

// 攻撃サウンド処理
void PlayerBase::ProcessAttackSound(int attackIndex, std::vector<AttackConfig> configs)
{
	// 攻撃設定からサウンド名を取得
	AttackConfig& config = configs[attackIndex];

	// サウンド名が空でない場合のみ
	if(!config.soundName.empty())
	{
		// サウンド再生
		SoundServer::GetInstance()->Play(config.soundName, DX_PLAYTYPE_BACK);
	}
}

// 攻撃分岐処理
void PlayerBase::ProcessBranchAttack()
{
	// 現在の状態に応じて攻撃処理を分岐
	int currentAttackIndex = GetAttackIndexByStatus(_playerState.attackState);

	if(currentAttackIndex >= 0 && currentAttackIndex < (static_cast<int>(_attacks.size())))
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
			if(_inputManager->IsTrigger(INPUT_ACTION::ATTACK) && CanNextAttack())
			{
				//ProcessNextAttack(attackIndex);
			}
			break;
		}

		case ATTACK_STATE::RECOVERY:	// 硬直中
		{
			// 次の攻撃入力があれば次の攻撃へ
			if(_inputManager->IsTrigger(INPUT_ACTION::ATTACK) && CanNextAttack())
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

	// 古いステータスを保持
	PlayerState oldState = _playerState;

	// 状態リセット
	_playerState.attackState = PLAYER_ATTACK_STATE::NONE;
	_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;

	_iComboCount = 0;				// コンボカウントリセット
	_bCanCombo = false;				// コンボ不可にする

	// ステータス変更後、アニメーション切り替え
	_oldPlayerState = oldState;		// 古いステータスを攻撃状態に設定
	ProcessPlayAnimation();			// アニメーション切り替え実行
	_oldPlayerState = _playerState;	// 切り替え後に更新
}

// 次の攻撃へ
void PlayerBase::ProcessNextAttack(int currentIndex)
{
	// 次の攻撃インデックスを計算
	int nextIndex = currentIndex + 1;

	// 次の攻撃が存在する場合
	if(nextIndex < static_cast<int>(_attacks.size()))
	{
 		PLAYER_ATTACK_STATE nextStatus = _attackStatuses[nextIndex];						// 次の状態取得
		ProcessStartAttack(nextIndex + 1, _attackStatuses[nextIndex], _attacks[nextIndex]);	// 次の攻撃へ
	}
}

// 攻撃コリジョンの情報受け取り用関数
void PlayerBase::ReceiveAttackColData()
{
	// 現在の攻撃インデックスを取得
	int attackIndex = GetAttackIndexByStatus(_playerState.attackState);

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

// 攻撃を開始できるかチェック
bool PlayerBase::IsStartAttack()
{
	if (!_inputManager) { return false; }

	// 攻撃入力チェック
	if((_playerState.movementState == PLAYER_MOVEMENT_STATE::WAIT ||	// 待機か
		_playerState.movementState == PLAYER_MOVEMENT_STATE::WALK ||	// 歩きか
		_playerState.movementState == PLAYER_MOVEMENT_STATE::RUN) &&	// 走りで
		_playerState.attackState   == PLAYER_ATTACK_STATE::NONE	  &&	// 攻撃状態ではなく
		_inputManager->IsTrigger(INPUT_ACTION::ATTACK))												// 入力があるなら
	{
		return true;
	}

	return false;
}

// 次の攻撃が可能かチェック
bool PlayerBase::CanNextAttack()
{
	// コンボが可能で、最大コンボ数以下の場合のみ次の攻撃可能
	AttackConstants constants = GetAttackConstants();

	// キャラタイプに応じた最大コンボ数を取得
	int maxComboCount = GetMaxComboCount();

	// コンボ可能で、現在のコンボカウントが最大コンボ数より小さいなら次の攻撃可能
	return _bCanCombo && _iComboCount < maxComboCount;
}

// 攻撃状態中かどうかをチェック
bool PlayerBase::IsAttacking()
{
	// 攻撃状態中かチェック
	if(_playerState.attackState == PLAYER_ATTACK_STATE::FIRST_ATTACK	||
		_playerState.attackState == PLAYER_ATTACK_STATE::SECOND_ATTACK	||
		_playerState.attackState == PLAYER_ATTACK_STATE::THIRD_ATTACK	||
		_playerState.attackState == PLAYER_ATTACK_STATE::FOURTH_ATTACK	||
		_playerState.attackState == PLAYER_ATTACK_STATE::FIFTH_ATTACK)
	{
		//_vMove = VGet(0, 0, 0);	// 攻撃中は移動不可
		return true;
	}

	return false;
}

// 攻撃入力があったかチェック
bool PlayerBase::IsAttackInput()
{
	// 攻撃入力があるかチェック
	return (_trg & PAD_INPUT_6) != 0;
}

// ヘルパー関数
std::shared_ptr<AttackBase> PlayerBase::GetAttackByStatus(PLAYER_ATTACK_STATE status)
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
			return IdConstants::INTERIOR_PLAYER_ID;	// 裏プレイヤーID
		}

		case CHARA_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			return IdConstants::SURFACE_PLAYER_ID;	// 表プレイヤーID
		}

		default:
		{
			return 0;
		}
	}
}

// 状態から攻撃インデックスを取得
int PlayerBase::GetAttackIndexByStatus(PLAYER_ATTACK_STATE status)
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

// 最大攻撃コンボ取得関数
int PlayerBase::GetMaxComboCount()const
{
	AttackConstants constants = GetAttackConstants();	// 攻撃定数取得

	// キャラタイプに応じた最大コンボ数を返す
	if(_eCharaType == CHARA_TYPE::SURFACE_PLAYER)
	{
		return constants.surfaceMaxComboCount;	// 表プレイヤー用最大コンボ数
	}
	else if(_eCharaType == CHARA_TYPE::INTERIOR_PLAYER)
	{
		return constants.interiorMaxComboCount;	// 裏プレイヤー用最大コンボ数
	}

	// デフォルトは0
	return 0;	
}
