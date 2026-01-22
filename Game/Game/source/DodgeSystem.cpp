#include "DodgeSystem.h"

namespace
{
	const float ZERO = 0.0f;				// ゼロ
	const float TIME_STEP = 1.0f / 60.0f;	// タイムステップ
}

DodgeSystem::DodgeSystem()
{
	_eDodgeState = DODGE_STATE::NONE;

	_currentTime = ZERO;		// 経過時間
	_invincibleTime = ZERO;		// 無敵経過時間
	_bIsDodging = false;		// 回避中フラグ
	_bIsInvincible = false;		// 無敵状態フラグ
}

DodgeSystem::~DodgeSystem()
{

}

bool DodgeSystem::Initialize()
{
	return true;
}

bool DodgeSystem::Terminate()
{
	return true;
}

bool DodgeSystem::Process()
{
	// 回避状態更新
	UpdateDodgeState();

	// 無敵状態更新
	UpdateInvincible();

	return true;
}

bool DodgeSystem::Render()
{
	return true;
}

// 回避設定初期化
bool DodgeSystem::InitializeConfig
(
	float invincibleDuration,   // 無敵持続時間
	float startTime,			// 開始時間
	float activeTime,			// アクティブ時間
	float recoveryTime,			// 硬直時間
	float dodgeMoveSpeed		// 回避移動速度
)
{
	// 回避設定初期化
	_stcDodgeConfig.invincibleDuration = invincibleDuration;	// 無敵持続時間
	_stcDodgeConfig.startTime = startTime;						// 開始時間
	_stcDodgeConfig.activeTime = activeTime;					// アクティブ時間
	_stcDodgeConfig.recoveryTime = recoveryTime;				// 硬直時間
	_stcDodgeConfig.dodgeMoveSpeed = dodgeMoveSpeed;			// 回避移動速度

	_eDodgeState = DODGE_STATE::INACTIVE;	// 回避状態初期化
	_currentTime = ZERO;					// 経過時間初期化

	return true;
}

// 回避呼び出し
void DodgeSystem::CallDodge
(
	std::shared_ptr<CharaBase>chara, 	// キャラオブジェクト
	DodgeConfig& config					// 回避設定		
)
{
	// 設定を初期化
	SetConfig(config);

	// 回避可能なら開始
	if(CanDodge())
	{
		// 回避開始
		StartDodge();

		// 回避位置更新
		UpdateDodgePos(chara);
	}
}

// 回避開始
void DodgeSystem::StartDodge()
{
	// 回避可能状態なら回避開始
	if(_eDodgeState == DODGE_STATE::INACTIVE)
	{
		_eDodgeState = DODGE_STATE::STARTUP;	// 回避状態を開始に設定
		_bIsDodging = true;						// 回避中フラグを立てる
		_bIsInvincible = false;					// 無敵状態フラグを下ろす
		_currentTime = ZERO;					// 経過時間リセット
		_invincibleTime = ZERO;					// 無敵経過時間リセット
	}
}

// 回避停止
void DodgeSystem::StopDodge()
{
	// 非アクティブ状態なら処理しない
	if(_eDodgeState == DODGE_STATE::INACTIVE){ return; }

	_eDodgeState = DODGE_STATE::RECOVERY;	// 回避状態を硬直に設定
	_bIsDodging = false;					// 回避中フラグを下ろす
	_bIsInvincible = false;					// 無敵状態フラグを下ろす
}

// 回避状態更新
void DodgeSystem::UpdateDodgeState()
{
	// 回避状態に応じて処理を分岐
	switch(_eDodgeState)
	{
		case DODGE_STATE::STARTUP:	// 開始
		{	
			_currentTime += TIME_STEP;	// 経過時間を進める

// 開始時間を超えたら実行中に移行
if(_currentTime >= _stcDodgeConfig.startTime)
{
	_eDodgeState = DODGE_STATE::ACTIVE;	// 回避状態を実行中に設定
	_bIsInvincible = true;				// 無敵状態フラグを立てる
	_currentTime = ZERO;				// 経過時間リセット
	_invincibleTime = ZERO;				// 無敵経過時間リセット
}

break;
		}

		case DODGE_STATE::ACTIVE:	// 実行中
		{
			_currentTime += TIME_STEP;	// 経過時間を進める

			// アクティブ時間を超えたら終了硬直に移行
			if(_currentTime >= _stcDodgeConfig.activeTime)
			{
				_eDodgeState = DODGE_STATE::RECOVERY;	// 回避状態を終了硬直に設定
				_bIsInvincible = false;					// 無敵状態フラグを下ろす
				_currentTime = ZERO;					// 経過時間リセット
			}

			break;
		}

		case DODGE_STATE::RECOVERY:	// 終了硬直
		{
			_currentTime += TIME_STEP;	// 経過時間を進める

			// 硬直時間を超えたら非アクティブに移行
			if(_currentTime >= _stcDodgeConfig.recoveryTime)
			{
				_eDodgeState = DODGE_STATE::INACTIVE;	// 回避状態を非アクティブに設定
				_bIsDodging = false;					// 回避中フラグを下ろす
				_currentTime = ZERO;					// 経過時間リセット
			}

			break;
		}
	}
}

// 回避位置更新
void DodgeSystem::UpdateDodgePos(std::shared_ptr<CharaBase>chara)
{
	if(chara == nullptr){ return; }

	if(IsDodging())
	{
		// 回避移動量計算
		VECTOR vDir = chara->GetDir();	// キャラの向き取得
		VECTOR vMove = VGet
		(
			cosf(vDir.y) * _stcDodgeConfig.dodgeMoveSpeed,
			ZERO,
			sinf(vDir.y) * _stcDodgeConfig.dodgeMoveSpeed
		);

		// キャラの移動量に回避移動量を加算
		VECTOR vCurrentMove = chara->GetMove();
		vCurrentMove.x += vMove.x;
		vCurrentMove.y += vMove.y;
		vCurrentMove.z += vMove.z;
		chara->SetMove(vCurrentMove);
	}
}

// 無敵状態更新
void DodgeSystem::UpdateInvincible()
{
	// 無敵時間の管理
	if(_bIsInvincible)
	{
		// 無敵経過時間を進める
		_invincibleTime += TIME_STEP;

		// 無敵持続時間を超えたら無敵状態解除
		if(_invincibleTime >= _stcDodgeConfig.invincibleDuration)
		{
			_bIsInvincible = false;
			_invincibleTime = ZERO;
		}
	}
}

// 回避可能かチェック
bool DodgeSystem::CanDodge() const
{
	return (_eDodgeState == DODGE_STATE::INACTIVE);
}

// 回避中かチェック
bool DodgeSystem::IsDodging() const
{
	// 回避状態が開始か実行中なら回避中とみなす
	if(_eDodgeState == DODGE_STATE::STARTUP || _eDodgeState == DODGE_STATE::ACTIVE)
	{
		// 回避中
		return true;
	}
	
	return false;
}