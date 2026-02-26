// 担当 : 成田

#include "SurfacePlayer.h"

namespace
{
	// 基礎ステータス定数
	constexpr float GRAVITY = -0.6f;		// 重力加速度
	constexpr float DEFAULT_LIFE = 100.0f;	// デフォルト体力
	constexpr float MAX_LIFE = 100.0f;		// 最大体力

	// 表示用定数
	constexpr int DRAW_SIZE_OFFSET = 16;	// 描画サイズオフセット
	constexpr int DRAW_OFFSET_X = 900;		// 描画Xオフセット
	constexpr int DRAW_OFFSET_Y = 0;		// 描画Yオフセット
}

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

// デバッグ描画
void SurfacePlayer::DebugRender()
{
	// コリジョン描画
	PlayerBase::DebugRender();

	// 吸収攻撃システムのデバッグ描画
	AbsorbSystemDebugRender();
}

// 被ダメージ処理
void SurfacePlayer::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	// 基底クラスの被ダメージ処理呼び出し
	PlayerBase::ApplyDamage(fDamage, eType, attackInfo);
}

// 弾での被ダメージ処理
void SurfacePlayer::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)
{
	// 基底クラスの弾での被ダメージ処理呼び出し
	PlayerBase::ApplyDamageByBullet(fDamage, chara);
}

// 表プレイヤーの情報設定
PlayerConfig SurfacePlayer::GetPlayerConfig()
{
	// 表プレイヤー用の設定
	PlayerConfig config;

	// 移動速度設定
	config.crouchMoveSpeed = 2.0f;				
	config.normalMoveSpeed = 5.0f;				
	config.dashMoveSpeed = 2.5f;			

	// 基礎ステータス
	config.life = 200.0f;
	config.maxLife = 200.0f;					
	config.startPos = VGet(230.0f, 0.0f, 0.0f);	

	// 表示設定
	config.drawSizeOffset = 16;					
	config.drawOffsetX = 900;					
	config.drawOffsetY = 0;						

	// モデル名
	config.modelName = "SurfacePlayer";			

	return config;
}

// 表プレイヤーのアニメーション設定
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
	//animation.attack.firstAttack	= "absorb_attack_00";
	//animation.attack.secondAttack	= "absorb_attack_01";
	//animation.attack.thirdAttack	= "absorb_attack_02";
	//animation.attack.fourthAttack	= "";
	//animation.attack.fifthAttack	= "";
	animation.attack.firstSkill		= ""; 
	animation.attack.secondSkill	= "";
	animation.absorb.absorbReady	= "absorb_ready_00";
	animation.absorb.absorbActive	= "absorb_active_00";
	animation.absorb.absorbEnd		= "absorb_end_00";
	animation.combat.transform		= "player_change_00";
	animation.combat.guard			= "";
	animation.combat.hit			= "player_damage_00";
	animation.combat.dodge			= "player_dodge_00";
	animation.combat.death			= "player_dead_00";

	return animation;
}

// 表示設定
RenderConfig SurfacePlayer::GetRenderConfig()
{
	// 表プレイヤー用の表示設定
	RenderConfig config;

	config.playerName = "Surface Player";				// プレイヤー名
	config.debugColor = COLOR_U8{ 0, 255, 255, 255 };	// デバッグ描画色

	return config;
}

// 攻撃判定のパラメーター
AttackConstants SurfacePlayer::GetAttackConstants()const
{
	// SurfacePlayer専用の攻撃定数
	AttackConstants constants;

	constants.attackOffsetScale = 85.0f;	// 攻撃判定オフセット倍率	
	//constants.surfaceMaxComboCount = 3;		// 表プレイヤー用コンボカウント
	constants.surfaceMaxComboCount = 0;		// 表プレイヤー用コンボカウント

	return constants;
}

// 攻撃判定の情報設定
void SurfacePlayer::GetAttackConfigs(AttackConfig configs[3])
{
	// 第1攻撃
	configs[0] = 
	{ 
		{0.0f, 90.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 50.0f, 0.0f},	// コリジョン下部位置
		30.0f,					// 半径
		20.0f,					// 発生フレーム
		25.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		10.0f,					// ダメージ
		ATTACK_STATE::ACTIVE,	// 攻撃状態
		3.0f,					// 攻撃中の移動速度
		false,					// 吹き飛ばし攻撃かどうか
		"SurfacePlayerAttack1",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
		"sPlayerFirstAttack",	// サウンド名
	};	

	// 第2攻撃
	configs[1] = 
	{ 
		{0.0f, 80.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 10.0f, 0.0f},  	// コリジョン下部位置
		40.0f,					// 半径
		20.0f,					// 発生フレーム
		15.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		10.0f,					// ダメージ
		ATTACK_STATE::ACTIVE,	// 攻撃状態
		3.0f,					// 攻撃中の移動速度
		false,					// 吹き飛ばし攻撃かどうか
		"SurfacePlayerAttack1",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
		"sPlayerFirstAttack",		// サウンド名
	};

	// 第3攻撃
	configs[2] =
	{ 
		{0.0f, 100.0f, 0.0f},	// コリジョン上部位置
		{0.0f, 0.0f, 0.0f},		// コリジョン下部位置
		25.0f,					// 半径
		20.0f,					// 発生フレーム
		25.0f,					// 持続フレーム
		20.0f,					// 硬直フレーム
		20.0f,					// ダメージ
		ATTACK_STATE::ACTIVE,	// 攻撃状態
		3.0f,					// 攻撃中の移動速度
		false,					// 吹き飛ばし攻撃かどうか
		"SurfacePlayerAttack3",	// エフェクト名
		{0.0f, 50.0f, 0.0f},	// エフェクト位置オフセット
		"iPlayerAttack",		// サウンド名
	};
}

// 範囲攻撃の情報設定
AreaAttackConfig SurfacePlayer::GetAreaAttackConfig()
{
	// 表プレイヤーは範囲攻撃を行わない
	AreaAttackConfig config;
	return config;
}

// 回避設定データ構造体
DodgeConfig SurfacePlayer::GetDodgeConfig()
{
	// 表プレイヤー用の回避設定
	DodgeConfig config;

	config.charaType = DODGE_CHARA::SURFACE_PLAYER;
	config.invincibleDuration = 20.0f;	// 無敵時間
	config.startTime = 25.0f;			// 開始時間
	config.activeTime = 30.0f;			// アクティブ時間
	config.recoveryTime = 25.0f;			// 硬直時間
	config.dodgeMoveSpeed = 12.0f;		// 移動速度

	return config;
}

// シールド設定を取得
ShieldConfig SurfacePlayer::GetShieldConfig()
{
	// デフォルトの設定を返す
	ShieldConfig config;

	config.charaType = SHIELD_CHARA::SURFACE_PLAYER;
	config.dotThreshold = 0.5f;
	config.blockAngleRange = 90.0f;
	config.blockDistance = 100.0f;
	config.startTime = 10.0f;
	config.recoveryTime = 15.0f;
	config.staminaCost = 10.0f;
	config.blockEffectName = "";
	config.effectOffset = 0.0f;

	return config;
}

// 吸収攻撃システムの初期化
void SurfacePlayer::MakeAbsorbSystem()
{
	_absorbAttackSystem = std::make_unique<PlayerAbsorbAttackSystem>();
}

// 吸収攻撃システムの初期化
void SurfacePlayer::InitializeAbsorbSystem()
{
	// 吸収システムの初期化
	if(_absorbAttackSystem)
	{
		_absorbAttackSystem->Initialize(shared_from_this());		// 所有者を設定して初期化
		_absorbAttackSystem->SetAbsorbConfig(GetAbsorbConfig());	// 吸収攻撃の設定を取得して設定
	}
}

// 吸収攻撃システムの更新
void SurfacePlayer::ProcessAbsorbSystem()
{
	if(!_absorbAttackSystem) { return; }

	// 吸収攻撃モーションの切り替え条件処理
	ProcessChangeAbsorbMotion();

	// 入力処理
	_absorbAttackSystem->ProcessAbsorbInput(_key);	

	// 吸収システムの更新処理
	_absorbAttackSystem->Process();

	// 前フレームの吸収攻撃キーの状態を保存
	_bWasAbsorbKeyPressed = IsAbsorbInput();
}
	// 吸収攻撃システムの処理
	if(_absorbAttackSystem)
	{
		_absorbAttackSystem->ProcessAbsorbInput();	// 入力処理
		_absorbAttackSystem->Process();					// 吸収システムの更新処理
	}
}

// 吸収攻撃設定取得
AbsorbConfig SurfacePlayer::GetAbsorbConfig()
{
	AbsorbConfig config;

	config.absorbRate = 1.0f;							// 吸収率
	config.energyAbsorbRate = 10.0f;					// エネルギー吸収率
	config.hpAbsorbRate = 10.0f;						// HP吸収率
	config.absorbRange = 120.0f;						// 吸収範囲
	config.absorbAngle = DX_PI_F / 2.0f;				// 90度の扇形(ラジアン)
	config.absorbDivision = 10;							// 滑らかな描画用
	config.absorbEffectName = "SurfacePlayerAbsorb";	// 吸収エフェクト名

	return config;
}

// 吸収攻撃システム取得
const PlayerAbsorbAttackSystem* SurfacePlayer::GetAbsorbAttackSystem()const
{
	return _absorbAttackSystem.get();
}

// 吸収攻撃システムのデバッグ描画
void SurfacePlayer::AbsorbSystemDebugRender()
{
	if(_absorbAttackSystem)
	{
		_absorbAttackSystem->DebugRender();
	}

	// 吸収アニメーション再生時間のデバッグ表示
	DebugDrawAbsorbAnimationTime();
}

// 吸収攻撃モーション切り替え条件処理
void SurfacePlayer::ProcessChangeAbsorbMotion()
{
	// 吸収入力開始
	if(IsAbsorbInput() && !_bWasAbsorbKeyPressed)
	{
		// 構え状態に移行
		StartAbsorbReadyState();
	}
	// 吸収入力を続けているなら
	else if(IsAbsorbing() && _playerState.absorbState == PLAYER_ABSORB_STATE::ABSORB_READY)
	{
		AnimManager* animManager = GetAnimManager();

		// 構えモーションが終了したかチェック
		if(animManager->IsAnimationFinished())
		{
			// 構えモーション終了時の処理
			ProcessAbsorbReadyCompleted();
		}
	}
	// 吸収入力が終了しているなら
	else if(!IsAbsorbInput() && _bWasAbsorbKeyPressed)
	{
		// 吸収攻撃終了処理
		ProcessAbsorbFinish();
	}
}

// 構え状態に移行
void SurfacePlayer::StartAbsorbReadyState()
{
	// 吸収構え状態に移行
	_playerState.absorbState = PLAYER_ABSORB_STATE::ABSORB_READY;	// 状態を吸収構えに設定
	_bIsAbsorbReadyCompleted = false;								// 吸収構えアニメーション完了フラグをリセット

	// アニメーション切り替え
	ProcessPlayAnimation();
}

// 構えモーション終了時の処理
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

// 吸収終了処理
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

// 吸収停止処理
void SurfacePlayer::StopAbsorb()
{
	// 吸収攻撃停止処理
	_absorbAttackSystem->StopAbsorbAttack();

	// 状態を吸収終了にする
	_playerState.absorbState = PLAYER_ABSORB_STATE::ABSORB_END;

	// アニメーション切り替え
	ProcessPlayAnimation();
}

// 吸収構えキャンセル処理
void SurfacePlayer::CancelAbsorbReady()
{
	// 状態を吸収終了にする
	_playerState.absorbState = PLAYER_ABSORB_STATE::NONE;

	// アニメーション切り替え
	ProcessPlayAnimation();
}

// 吸収攻撃の入力チェック
bool SurfacePlayer::IsAbsorbInput()const
{
	// 吸収攻撃の入力チェック
	return (_key & PAD_INPUT_6) != 0;
}

// 吸収アニメーション再生時間デバッグ表示
void SurfacePlayer::DebugDrawAbsorbAnimationTime()
{
	if(!_absorbAttackSystem) { return; }

	AnimManager* animManager = GetAnimManager();
	if(!animManager) { return; }

	// 吸収アニメーション再生時間のデバッグ表示
	std::string debugText = "AbsorbAnimTime: " + std::to_string(animManager->GetCurrentAnimTotalTime());

	// 吸収攻撃のアニメーション再生時間をデバッグ表示
	DrawFormatString(DRAW_OFFSET_X, DRAW_OFFSET_Y + 100, GetColor(255, 255, 255), debugText.c_str());
}