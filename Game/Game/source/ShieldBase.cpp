#include "ShieldBase.h"
#include"CharaBase.h"
#include "StaminaManager.h"

namespace
{
	constexpr float DECREMENT_COUNT = 1.0f;
	constexpr float SHIELD_COST_STAMINA = 0.5f;
}

ShieldBase::ShieldBase()
{
	_eShieldChara = SHIELD_CHARA::NONE;
	_eShieldState = SHIELD_STATE::INACTIVE;

	_fStartupTimer = 0.0f;				// 発生前タイマー
	_fRecoveryTimer = 0.0f;				// 硬直タイマー
	_bIsBlocking = false;				// ブロック中フラ
	_stcShieldConfig = ShieldConfig();	// シールド設定初期化
}

ShieldBase::~ShieldBase()
{

}

bool ShieldBase::Initialize()
{
	return true;
}

bool ShieldBase::Terminate()
{
	return true;
}

bool ShieldBase::Process()
{
	// シールド状態更新
	UpdateShieldState();

	return true;
}

bool ShieldBase::Render()
{
	return true;
}

// シールド発動
void ShieldBase::ActivateShield()
{
	// 非アクティブ状態なら
	if(_eShieldState == SHIELD_STATE::INACTIVE)
	{
		// アクティブに移行
		_eShieldState = SHIELD_STATE::STARTUP;			// シールド状態を発生前に設定
		_fStartupTimer = _stcShieldConfig.startTime;	// 発生前タイマーセット
	}
}

// シールド解除
void ShieldBase::DeactivateShield()
{
	// アクティブ中なら
	if(_eShieldState == SHIELD_STATE::ACTIVE)
	{
		_eShieldState = SHIELD_STATE::RECOVERY;				// シールド状態を非アクティブに設定
		_fRecoveryTimer = _stcShieldConfig.recoveryTime;	// 硬直タイマーセット
	}
	// 発生前状態なら
	else if(_eShieldState == SHIELD_STATE::STARTUP)
	{
		_eShieldState = SHIELD_STATE::INACTIVE;				// シールド状態を硬直に設定
		_fStartupTimer = 0.0f;								// 発生前タイマーリセット
	}
}

// 攻撃をブロック可能か
bool ShieldBase::CanBlockAttack(const VECTOR& attackPos, const VECTOR& attackDir) const
{
	// シールドがアクティブでない場合はブロック不可
	if(!IsActive()) { return false; }

	// 所有者がいない場合はブロック不可
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// 所有者位置と前方ベクトル取得
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// 攻撃位置がシールド有効範囲内かチェック
	if(!IsInShieldRange(attackPos)) { return false; }

	// 攻撃が有効な方向からかチェック
	if(!IsAttackFromValidDirection(attackDir)) { return false; }

	// すべての条件を満たしている場合はブロック可能
	return GeometryUtility::IsAttackFromFront(ownerPos, ownerDir, attackDir, _stcShieldConfig.dotThreshold);
}

// 対象がシールド有効範囲内かチェック
bool ShieldBase::IsInShieldRange(const VECTOR& targetPos) const
{
	// 所有者がいない場合はブロック不可
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// 所有者位置取得
	VECTOR ownerPos = GetOwnerPos();

	// 所有者から対象への距離を計算
	float distance = VSize(VSub(targetPos, ownerPos));

	// シールド有効距離内かチェック
	return distance <= _stcShieldConfig.blockDistance;
}

// 攻撃が有効な方向からかチェック
bool ShieldBase::IsAttackFromValidDirection(const VECTOR& attackDir) const
{
	// 所有者がいない場合はブロック不可
	auto owner = GetOwner();
	if(!owner) { return false; } 

	// 所有者位置と向き
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// 攻撃者から所有者への方向
	VECTOR attackToOwner = VScale(attackDir, -1.0f);

	// 所有者前方と攻撃方向の角度差を計算
	float attackDiff = GeometryUtility::GetAngleDeg(ownerDir, attackToOwner);

	// 角度範囲内かチェック
	return attackDiff <= _stcShieldConfig.blockAngleRange;
}

// ブロック成功処理
void ShieldBase::SuccessBlock(const VECTOR& attackDir)
{
	_bIsBlocking = true;	// ブロック中
}

// ガード入力処理
void ShieldBase::UpdateGuardInput(int key)
{
	// 押されたら
	if(key & PAD_INPUT_2)
	{
		// ガード開始
		StartGuard();
	}
	// 離されたら
	else
	{
		// ガード終了
		StopGuard();
	}
}

// ガード開始
void ShieldBase::StartGuard()
{
	if(!HasStamina()) { return; }	// スタミナチェック

	// シールド発動
	ActivateShield();
}

// ガード停止
void ShieldBase::StopGuard()
{
	// シールド解除
	DeactivateShield();
}

// スタミナが足りているかチェック
bool ShieldBase::HasStamina() const
{
	return StaminaManager::GetInstance()->CanShield();
}

// シールド状態更新
void ShieldBase::UpdateShieldState()
{
	switch(_eShieldState)
	{
		case SHIELD_STATE::STARTUP: // 発生前
		{
			_fStartupTimer -= DECREMENT_COUNT;	// 硬直タイマーを進める

			if(_fStartupTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::ACTIVE;		// シールド状態をアクティブに設定
			}

			break;
		}

		case SHIELD_STATE::ACTIVE: // アクティブ中
		{
			// スタミナ消費
			StaminaManager::GetInstance()->ConsumeStamina(SHIELD_COST_STAMINA);

			break;
		}

		case SHIELD_STATE::RECOVERY: // 硬直中
		{
			_fRecoveryTimer -= DECREMENT_COUNT;	// 硬直タイマーを進める

			// 硬直時間を超えたら
			if(_fRecoveryTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::INACTIVE;	// シールド状態を非アクティブに設定
				_bIsBlocking = false;					// ブロック中フラグ解除
			}

			break;
		}
	}
}

// 所有者位置取得
VECTOR ShieldBase::GetOwnerPos() const
{
	auto owner = GetOwner();
	if(owner)
	{
		// 所有者位置取得
		return owner->GetPos();
	}

	// 所有者がいない場合はゼロ
	return VGet(0.0f, 0.0f, 0.0f);
}

// 所有者前方取得
VECTOR ShieldBase::GetOwnerDir() const
{
	auto owner = GetOwner();
	if(owner)
	{
		// 所有者前方取得
		return owner->GetDir();
	}
	
	// 所有者がいない場合はゼロ
	return VGet(0.0f, 0.0f, 0.0f);
}

// デバッグ表示
void ShieldBase::DebugRender()
{
	// シールド有効範囲デバッグ描画
	DrawShieldRangeDebug();
}

// シールド有効範囲デバッグ描画
void ShieldBase::DrawShieldRangeDebug()
{
	if(_eShieldState != SHIELD_STATE::ACTIVE){ return; }

	auto owner = GetOwner();
	if(!owner) { return; } 

	// 所有者位置と前方ベクトル取得
	VECTOR ownerPos = GetOwnerPos();
	VECTOR ownerDir = GetOwnerDir();

	// シールド有効範囲描画

}


