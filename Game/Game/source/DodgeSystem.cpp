#include "DodgeSystem.h"

namespace DodgeContants
{
	constexpr float TIME_STEP = 1.0f;	// タイムステップ
}

DodgeSystem::DodgeSystem()
{
	_eDodgeChara = DODGE_CHARA::NONE;		// 回避キャラ初期化
	_eDodgeState = DODGE_STATE::INACTIVE;	// 回避状態初期化

	_currentTime = 0.0f;		// 経過時間
	_invincibleTime = 0.0f;		// 無敵経過時間
	_bIsDodging = false;		// 回避中フラグ
	_bIsInvincible = false;		// 無敵状態フラグ

	_stcDodgeConfig = DodgeConfig();	// 回避設定初期化
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

void DodgeSystem::RegisterDodgeCharaConfig(DODGE_CHARA charaType, const DodgeConfig& config)
{
	_charaConfigs[charaType] = config;
}

void DodgeSystem::CallDodge
(
	std::shared_ptr<CharaBase> chara,
	DODGE_CHARA charaType
)
{
	// キャラオブジェクトが無効なら処理しない
	if(!chara){ return; }	

	// キャラタイプの設定を取得
	auto config = _charaConfigs.find(charaType);
	if(config == _charaConfigs.end()){ return; }

	// キャラタイプの有効性チェック
	if(charaType == DODGE_CHARA::NONE){ return; }
	
	// 回避キャラと設定を保存
	_currentDodgeChara = chara;			// 回避中キャラ保存
	_eDodgeChara = charaType;			// 回避キャラ設定
	_stcDodgeConfig = config->second;	// 回避設定保存

	// 回避可能なら開始
	if(CanDodge())
	{
		// 回避開始
		StartDodge();

		// 保存された回避設定のサウンド名から再生
		SoundServer::GetInstance()->Play(_stcDodgeConfig.soundName, DX_PLAYTYPE_BACK);
	}
}

void DodgeSystem::StartDodge()
{
	// 回避可能状態なら回避開始
	if(_eDodgeState == DODGE_STATE::INACTIVE)
	{
		_eDodgeState = DODGE_STATE::STARTUP;	// 回避状態を開始に設定
		_bIsDodging = true;						// 回避中フラグを立てる
		_bIsInvincible = false;					// 無敵状態フラグを下ろす
		_currentTime = 0.0f;					// 経過時間リセット
		_invincibleTime = 0.0f;					// 無敵経過時間リセット
	}
}

void DodgeSystem::StopDodge()
{
	// 非アクティブ状態なら処理しない
	if(_eDodgeState == DODGE_STATE::INACTIVE){ return; }

	_eDodgeState = DODGE_STATE::RECOVERY;	// 回避状態を硬直に設定
	_bIsDodging = false;					// 回避中フラグを下ろす
	_bIsInvincible = false;					// 無敵状態フラグを下ろす
}

void DodgeSystem::UpdateDodgeState()
{
	// 現在のキャラ
	auto chara = _currentDodgeChara.lock();

	// 回避状態に応じて処理を分岐
	switch(_eDodgeState)
	{
		case DODGE_STATE::STARTUP:	// 開始
		{	
			_currentTime += DodgeContants::TIME_STEP;	// 経過時間を進める

			// 開始時間を超えたら実行中に移行
			if(_currentTime >= _stcDodgeConfig.startTime)
			{
				_eDodgeState = DODGE_STATE::ACTIVE;	// 回避状態を実行中に設定
				_bIsInvincible = true;				// 無敵状態フラグを立てる
				_currentTime = 0.0f;				// 経過時間リセット
				_invincibleTime = 0.0f;				// 無敵経過時間リセット

				// キャラタイプ別の追加処理
				if((_eDodgeChara == DODGE_CHARA::SURFACE_PLAYER && chara ||
					_eDodgeChara == DODGE_CHARA::INTERIOR_PLAYER && chara ||
					_eDodgeChara == DODGE_CHARA::BULLET_PLAYER && chara))
				{
					// 回避エフェクト等
				}
			}
			
			break;
		}

		case DODGE_STATE::ACTIVE:	// 実行中
		{
			_currentTime += DodgeContants::TIME_STEP;	// 経過時間を進める

			// 回避中キャラの回避位置更新
			auto chara = _currentDodgeChara.lock();
			if(chara){ UpdateDodgePos(chara); }

			// アクティブ時間を超えたら終了硬直に移行
			if(_currentTime >= _stcDodgeConfig.activeTime)
			{
				_eDodgeState = DODGE_STATE::RECOVERY;	// 回避状態を終了硬直に設定
				_currentTime = 0.0f;					// 経過時間リセット
			}

			break;
		}

		case DODGE_STATE::RECOVERY:	// 終了硬直
		{
			_currentTime += DodgeContants::TIME_STEP;	// 経過時間を進める

			// 硬直時間を超えたら非アクティブに移行
			if(_currentTime >= _stcDodgeConfig.recoveryTime)
			{
				_eDodgeState = DODGE_STATE::INACTIVE;	// 回避状態を非アクティブに設定
				_eDodgeChara = DODGE_CHARA::NONE;		// 回避キャラ設定リセット
				_bIsDodging = false;					// 回避中フラグを下ろす
				_bIsInvincible = false;					// 無敵状態フラグを下ろす
				_currentTime = 0.0f;					// 経過時間リセット
				_invincibleTime = 0.0f;					// 無敵経過時間リセット
				_currentDodgeChara.reset();				// 回避中のキャラリセット
			}

			break;
		}
	}
}

void DodgeSystem::UpdateDodgePos(std::shared_ptr<CharaBase>chara)
{
	if(chara == nullptr){ return; }	// キャラオブジェクトが無効なら処理しない
	if(!IsDodging()){ return; }		// 回避中でなければ処理しない

	VECTOR vDir = chara->GetDir();										// キャラの向き取得
	VECTOR dodgeDir = VGet(vDir.x, 0.0f, vDir.z);						// 水平方向の回避方向ベクトル計算
	VECTOR normDir = VNorm(dodgeDir);									// 正規化
	VECTOR dodgeMove = VScale(normDir, _stcDodgeConfig.dodgeMoveSpeed);	// 回避移動ベクトル計算

	// キャラの位置更新
	VECTOR currentPos = chara->GetPos();					// キャラの現在位置取得
	VECTOR newPos = VAdd(currentPos, dodgeMove);			// 新しい位置計算
	chara->SetPos(newPos);									// キャラの位置更新
	chara->SetMove(VGet(0.0f, chara->GetMove().y, 0.0f));	// 移動量はリセット

	// モデル位置更新
	AnimManager* animManager = chara->GetAnimManager();
	if(animManager != nullptr)
	{
		MV1SetPosition(animManager->GetModelHandle(), newPos);
	}

	// コリジョン位置を強制的に更新
	UpdateDodgeCharaCol(chara, newPos);
}

void DodgeSystem::UpdateDodgeCharaCol(std::shared_ptr<CharaBase>chara, const VECTOR& pos)
{
	if(chara == nullptr){ return; }

	chara->SetCollisionTop(VAdd(pos, VGet(0, 140, 0)));		// コリジョン位置を更新
	chara->SetCollisionBottom(VAdd(pos, VGet(0, 10, 0)));	// コリジョン位置を更新
}

void DodgeSystem::UpdateInvincible()
{
	// 無敵時間の管理
	if(_bIsInvincible)
	{
		// 無敵経過時間を進める
		_invincibleTime += DodgeContants::TIME_STEP;

		// 無敵持続時間を超えたら無敵状態解除
		if(_invincibleTime >= _stcDodgeConfig.invincibleDuration)
		{
			_bIsInvincible = false;
			_invincibleTime = 0.0f;
		}
	}
}

bool DodgeSystem::CanDodge() const
{
	return (_eDodgeState == DODGE_STATE::INACTIVE);
}

bool DodgeSystem::IsDodging() const
{
	// 回避中なら
	if((_eDodgeState == DODGE_STATE::STARTUP ||
		_eDodgeState == DODGE_STATE::ACTIVE	 ||
		_eDodgeState == DODGE_STATE::RECOVERY))
	{
		// 回避中
		return true;
	}
	
	return false;
}

bool DodgeSystem::IsCharacterInvincible(std::shared_ptr<CharaBase> chara) const
{
	if(!chara || !_bIsInvincible){ return false; }	// キャラが無効または無敵状態でなければ無敵ではない

	// 現在回避中のキャラと比較
	auto currentChara = _currentDodgeChara.lock();
	if(currentChara && currentChara == chara)
	{
		// 無敵状態
		return true;	
	}

	return false;
}

void DodgeSystem::DebugRender()
{
	// 無敵時間の表示
	if(_invincibleTime > 0.0f)
	{
		char buffer[256];
		sprintf_s(buffer, "Invincible Time: %3.2f / %3.2f", _invincibleTime, _stcDodgeConfig.invincibleDuration);
		DrawString(1100, 290, buffer, GetColor(255, 255, 255));
	}

	// 無敵中の当たり判定デバッグ描画
	InvincibleDebugRender();
}

// 無敵中の当たり判定デバッグ描画
void DodgeSystem::InvincibleDebugRender()
{
	if(!_bIsInvincible){ return; }

	auto chara = _currentDodgeChara.lock();
	if(!chara) { return; }

	// キャラクターのコリジョン情報を取得
	VECTOR charaTop = chara->GetCollisionTop();
	VECTOR charaBottom = chara->GetCollisionBottom();
	float charaRadius = chara->GetCollisionR();

	// 無敵コリジョンを少し大きく表示
	float invincibleRadius = charaRadius * 1.2f;

	// 無敵時間中のカプセルコリジョンを青色で描画
	DrawCapsule3D
	(
		charaBottom,
		charaTop,
		invincibleRadius,
		8,
		GetColor(0, 0, 255),	// 青色
		GetColor(0, 150, 255),	// 水色
		FALSE
	);
}