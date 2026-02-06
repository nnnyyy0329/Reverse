#include "ShieldBase.h"
#include"CharaBase.h"

ShieldBase::ShieldBase()
{
	_eShieldChara = SHIELD_CHARA::NONE;
	_eShieldState = SHIELD_STATE::INACTIVE;

	_fStartupTimer = 0.0f;	// 発生タイマー 
	_fActiveTimer = 0.0f;	// アクティブタイマー
	_fRecoveryTimer = 0.0f;	// 硬直タイマー

	_bIsBlocking = false;	// ブロック中フラ
}

ShieldBase::~ShieldBase()
{

}

bool ShieldBase::Initialize()
{
	// シールド設定取得
	_config = GetShieldConfig();

	return true;
}

bool ShieldBase::Terminate()
{
	return true;
}

bool ShieldBase::Process()
{
	// タイマー更新
	UpdateTimers();

	// シールド状態更新
	UpdateShieldState();

	return true;
}

bool ShieldBase::Render()
{
	return true;
}

// デバッグ描画
void ShieldBase::DebugRender() const
{

}

// シールド発動
bool ShieldBase::ActivateShield()
{
	if(!CanActivateShield()) { return false; }

	_eShieldState = SHIELD_STATE::STARTUP;	// シールド状態を発生前に設定
	_fStartupTimer = _config.startTime;		// 発生タイマー初期化

	return true;
}

// シールド解除
void ShieldBase::DeactivateShield()
{
	_eShieldState = SHIELD_STATE::INACTIVE;	// シールド状態を非アクティブに設定
	_fStartupTimer = 0.0f;					// 発生タイマー初期化
	_fActiveTimer = 0.0f;					// アクティブタイマー初期化
	_fRecoveryTimer = 0.0f;					// 硬直タイマー初期化
	_bIsBlocking = false;					// ブロック中フラグ解除
}

// シールド発動可能かチェック
bool ShieldBase::CanActivateShield() const
{
	// シールドが非アクティブ状態の場合のみ発動可能
	return (_eShieldState == SHIELD_STATE::INACTIVE);
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
	return GeometryUtility::IsAttackFromFront(ownerPos, ownerDir, attackDir, _config.dotThreshold);
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
	return distance <= _config.blockDistance;
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
	return attackDiff <= _config.blockAngleRange;
}

// ブロック成功処理
void ShieldBase::SuccessBlock(const VECTOR& attackDir)
{
	_bIsBlocking = true;	// ブロック中
}

// タイマー更新
void ShieldBase::UpdateTimers()
{
	switch(_eShieldState)
	{
		case SHIELD_STATE::STARTUP: // 発生前
		{
			_fStartupTimer -= 1.0f;
			break;
		}

		case SHIELD_STATE::ACTIVE: // アクティブ中
		{
			_fActiveTimer -= 1.0f;
			break;
		}

		case SHIELD_STATE::RECOVERY: // 硬直中
		{
			_fRecoveryTimer -= 1.0f;
			break;
		}
	}
}

// シールド状態更新
void ShieldBase::UpdateShieldState()
{
	switch(_eShieldState)
	{
		case SHIELD_STATE::STARTUP: // 発生前
		{
			if(_fStartupTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::ACTIVE;	// シールド状態をアクティブに設定
				_fActiveTimer = _config.activeTime;	// アクティブタイマー初期化
			}
			break;
		}
		case SHIELD_STATE::ACTIVE: // アクティブ中
		{
			if(_fActiveTimer <= 0.0f)
			{
				_eShieldState = SHIELD_STATE::RECOVERY;	// シールド状態を硬直中に設定
				_fRecoveryTimer = _config.recoveryTime;	// 硬直タイマー初期化
			}
			break;
		}
		case SHIELD_STATE::RECOVERY: // 硬直中
		{
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

	return VGet(0.0f, 0.0f, 0.0f);
}

// デバッグ表示
void ShieldBase::DebugRender() const
{

}


