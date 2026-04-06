// 担当 : 成田

#include "SurfacePlayer.h"

// 表プレイヤー用定数のエイリアス
namespace SPC = SurfacePlayerConstants;

SurfacePlayer::SurfacePlayer()
{
	// キャラタイプ
	SetCharaType(CHARA_TYPE::SURFACE_PLAYER);

	// 吸収攻撃システムの生成
	MakeAbsorbSystem();

	// 吸収攻撃のモーション管理用変数の初期化
	_bIsAbsorbReadyCompleted = false;	// 吸収構えアニメーションが完了したかどうか
	_bWasAbsorbKeyPressed = false;		// 前フレームで吸収攻撃キーが押されていたかどうか
}

SurfacePlayer::~SurfacePlayer()
{

}

bool SurfacePlayer::Initialize()
{
	// 基底クラスの初期化
	PlayerBase::Initialize();

	// 吸収攻撃システムの初期化
	InitializeAbsorbSystem();

	return true;
}

bool SurfacePlayer::Terminate()
{
	return true;
}

bool SurfacePlayer::Process()
{
	// 基底クラスの更新処理
	PlayerBase::Process();

	// 吸収攻撃システムの更新
	ProcessAbsorbSystem();

	return true;
}

bool SurfacePlayer::Render()
{
	// 基底クラスの描画処理呼び出し
	PlayerBase::Render();

	return true;
}

void SurfacePlayer::DebugRender()
{
	// コリジョン描画
	PlayerBase::DebugRender();

	// 吸収攻撃システムのデバッグ描画
	AbsorbSystemDebugRender();
}

void SurfacePlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE ownerType, const AttackCollision& attackInfo)
{
	// 表プレイヤー専用のダメージ倍率を適用
	float resultDamage = fDamage * SPC::DAMAGE_MULTIPLIER;

	// 基底クラスの被ダメージ処理呼び出し
	PlayerBase::ApplyDamage(resultDamage, ownerType, attackInfo);
}

void SurfacePlayer::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)
{
	// 基底クラスの弾での被ダメージ処理呼び出し
	PlayerBase::ApplyDamageByBullet(fDamage, chara);
}

PlayerConfig SurfacePlayer::GetPlayerConfig()
{
	// 表プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed	= 2.0f;				
	config.normalMoveSpeed	= 3.0f;				
	config.dashMoveSpeed	= 2.5f;			

	// 基礎ステータス
	config.life		= 200.0f;
	config.maxLife	= 200.0f;					

	// 表示設定
	config.drawSizeOffset	= 16;					
	config.drawOffsetX		= 900;					
	config.drawOffsetY		= 0;						

	// モデル名
	config.modelName = "SurfacePlayer";			

	return config;
}

PlayerAnimations SurfacePlayer::GetPlayerAnimation()
{
	// 表プレイヤー用のアニメーション設定
	PlayerAnimations animation;

	animation.movement.wait			= "player_idle_00";
	animation.movement.walk			= "player_walk_00";
	animation.movement.run			= "player_jog_00";
	animation.movement.jumpUp		= "";
	animation.movement.jumpDown		= "";
	animation.movement.crouchWait	= "";
	animation.movement.crouchWalk	= "";
	animation.attack.firstSkill		= ""; 
	animation.attack.secondSkill	= "";
	animation.absorb.absorbReady	= "player_absorb_00";
	animation.absorb.absorbActive	= "player_absorb_01";
	animation.absorb.absorbEnd		= "player_absorb_02";
	animation.combat.transform		= "player_change_00";
	animation.combat.guard			= "";
	animation.combat.hit			= "player_damage_00";
	animation.combat.dodge			= "player_dodge_00";
	animation.combat.death			= "player_dead_00";

	return animation;
}

RenderConfig SurfacePlayer::GetRenderConfig()
{
	// 表プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Surface Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

DodgeConfig SurfacePlayer::GetDodgeConfig()
{
	// 表プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::SURFACE_PLAYER;
	config.invincibleDuration = 20.0f;			// 無敵時間
	config.startTime = 2.0f;					// 開始時間
	config.activeTime = 40.0f;					// アクティブ時間
	config.recoveryTime = 10.0f;				// 硬直時間
	config.dodgeMoveSpeed = 10.0f;				// 移動速度
	config.soundName = "SE_SurfacePlayerDodge";	// サウンド名

	return config;
}

//ShieldConfig SurfacePlayer::GetShieldConfig()
//{
//	// デフォルトの設定を返す
//	ShieldConfig config;
//
//	config.charaType = SHIELD_CHARA::SURFACE_PLAYER;
//	config.dotThreshold = 0.5f;
//	config.blockAngleRange = 90.0f;
//	config.blockDistance = 100.0f;
//	config.startTime = 10.0f;
//	config.recoveryTime = 15.0f;
//	config.staminaCost = 10.0f;
//	config.blockEffectName = "";
//	config.effectOffset = 0.0f;
//
//	return config;
//}

void SurfacePlayer::MakeAbsorbSystem()
{
	// 吸収攻撃システムの生成
	_absorbAttackSystem = std::make_unique<PlayerAbsorbAttackSystem>();
}

void SurfacePlayer::InitializeAbsorbSystem()
{
	if(!_absorbAttackSystem) { return; }

	// 所有者を設定して初期化
	// CharaBase の shared_from_this() を使用するため、Initialize関数内で所有者を設定する必要がある
	_absorbAttackSystem->Initialize(shared_from_this());

	// 吸収攻撃の設定を取得して設定
	_absorbAttackSystem->SetAbsorbConfig(GetAbsorbConfig());	
}

void SurfacePlayer::ProcessAbsorbSystem()
{
	if(!_absorbAttackSystem){ return; }

	// 回避状態の時は吸収の入力処理をスキップ
	if(_playerState.IsInCombatState(PLAYER_COMBAT_STATE::DODGE)){ return; }

	// 吸収攻撃モーションの切り替え条件の更新
	UpdateChangeAbsorbMotion();

	// 入力処理
	_absorbAttackSystem->ProcessAbsorbInput();	

	// 吸収システムの更新処理
	_absorbAttackSystem->Process();

	// 前フレームの吸収攻撃キーの状態を保存
	_bWasAbsorbKeyPressed = IsAbsorbInput();
}

AbsorbConfig SurfacePlayer::GetAbsorbConfig()
{
	AbsorbConfig config;

	config.absorbRate		= 1.0f;						// 吸収率
	config.energyAbsorbRate = 10.0f;					// エネルギー吸収率
	config.hpAbsorbRate		= 10.0f;					// HP吸収率(未使用)
	config.absorbRange		= 240.0f;					// 吸収範囲
	config.absorbAngle		= DX_PI_F / 2.5f;			// 吸収角度
	config.absorbDivision	= 10;						// 滑らかな描画用
	config.absorbEffectName = "SurfacePlayerAbsorb";	// 吸収エフェクト名
	config.effectOffset		= VGet(0.0f, 0.0f, 0.0f);	// エフェクト位置オフセット
	config.effectRotOffset	= VGet(0.0f, 0.0f, 0.0f);	// エフェクト回転オフセット
	config.absorbSoundName	= "sPlayerAttack";			// 吸収サウンド名

	return config;
}

const PlayerAbsorbAttackSystem* SurfacePlayer::GetAbsorbAttackSystemConst()const
{
	return _absorbAttackSystem.get();
}

PlayerAbsorbAttackSystem* SurfacePlayer::GetAbsorbAttackSystem()
{
	return _absorbAttackSystem.get();
}

void SurfacePlayer::AbsorbSystemDebugRender()
{
	if(_absorbAttackSystem)
	{
		_absorbAttackSystem->DebugRender();
	}

	// 吸収アニメーション再生時間のデバッグ表示
	DebugDrawAbsorbAnimationTime();
}

void SurfacePlayer::UpdateChangeAbsorbMotion()
{
	// 吸収終了時に通常モーションに戻す処理
	ReturnNormalMotion();

	if(IsInputInAbsorbFinishState()){ return; }	// 吸収終了状態での入力は無視

	// 吸収入力開始
	if(IsAbsorbInput() && !_bWasAbsorbKeyPressed)
	{
		// 構え状態に移行
		StartAbsorbReadyState();
	}
	// 構え状態なら
	else if(_playerState.IsStateAbsorbing() && _playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_READY) 
	{
		// 構え状態中で入力が続いていないなら
		if(_playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_READY && !IsAbsorbInput())
		{
			// 吸収構えキャンセル処理
			StopAbsorb();

			return;
		}

		// まだ構えモーションが終了していないなら
		if(!IsAnimationFinishedConst()){ return; }

		// 構えモーション終了時の処理
		ProcessAbsorbReadyCompleted();
	}
	// 吸収入力が終了しているなら
	else if(!IsAbsorbInput() && _bWasAbsorbKeyPressed)
	{
		// 吸収攻撃終了処理
		ProcessAbsorbFinish();
	}
}

void SurfacePlayer::StartAbsorbReadyState()
{
	// 吸収構え状態に移行
	_playerState.absorbState = PLAYER_ABSORB_STATE::ABSORB_READY;	// 状態を吸収構えに設定
	_bIsAbsorbReadyCompleted = false;								// 吸収構えアニメーション完了フラグをリセット
	
	// アニメーション切り替え
	ProcessPlayAnimation();
}

void SurfacePlayer::ProcessAbsorbReadyCompleted()
{
	// 構えモーション終了
	_bIsAbsorbReadyCompleted = true;

	// 吸収攻撃開始
	_absorbAttackSystem->StartAbsorbAttack();

	// 吸収状態をアクティブにする
	_playerState.absorbState = PLAYER_ABSORB_STATE::ABSORB_ACTIVE;

	// アニメーション切り替え
	ProcessPlayAnimation();
}

void SurfacePlayer::ProcessAbsorbFinish()
{
	// 押していて吸収が始まっていたら停止
	if(_playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_ACTIVE)
	{
		// 吸収攻撃停止処理
		StopAbsorb();
	}
	// 構え状態で押していて、吸収が始まっていなかったらキャンセル
	else if(_playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_READY)
	{
		// 吸収構えキャンセル処理
		CancelAbsorbReady();
	}
}

void SurfacePlayer::StopAbsorb()
{
	// 吸収攻撃停止処理
	_absorbAttackSystem->StopAbsorbAttack();

	// 状態を吸収終了にする
	_playerState.absorbState = PLAYER_ABSORB_STATE::ABSORB_END;

	// アニメーション切り替え
	ProcessPlayAnimation();
}

void SurfacePlayer::CancelAbsorbReady()
{
	// 状態を吸収終了にする
	_playerState.absorbState = PLAYER_ABSORB_STATE::NONE;

	// アニメーション切り替え
	ProcessPlayAnimation();
}

void SurfacePlayer::ReturnNormalMotion()
{
	// 吸収終了状態なら通常モーションに戻す
	if(_playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_END)
	{
		// どちらの場合もアニメーションが終了していたら通常モーションに戻す
		if(IsAnimationFinishedConst())
		{
			// 通常モーションに戻す処理
			ProcessReturnNormalMotion();
		}
	}
}

void SurfacePlayer::DebugDrawAbsorbAnimationTime()
{
	if(!_absorbAttackSystem) { return; }

	// アニメーションマネージャーを取得
	AnimManager* animManager = GetAnimManager();
	if(!animManager) { return; }

	// 吸収アニメーション再生時間のデバッグ表示
	std::string debugText = "AbsorbAnimTime: " + std::to_string(animManager->GetCurrentAnimTotalTime());

	// 吸収攻撃のアニメーション再生時間をデバッグ表示
	DrawFormatString(10, 350, GetColor(255, 255, 255), debugText.c_str());
}