// 担当 : 成田

#include "PlayerBase.h"
#include "PlayerShieldSystem.h"

// 被弾設定
namespace HitConfig
{
	constexpr float HIT_SPEED = 15.0f;	// 被弾時の吹き飛び速度
	constexpr float HIT_DECAY = 0.9f;	// 被弾時の吹き飛び減衰率
	constexpr float HIT_TIME = 30.0f;	// 被弾時間
}

namespace
{
	const float GRAVITY = 0.6f;	// 重力定数
}

PlayerBase::PlayerBase()
{
	_cameraManager = nullptr;	// カメラマネージャー

	// キャラの状態初期化
	_playerState.movementState = PLAYER_MOVEMENT_STATE::NONE;
	_playerState.attackState = PLAYER_ATTACK_STATE::NONE;
	_playerState.shootState = PLAYER_SHOOT_STATE::NONE;
	_playerState.combatState = PLAYER_COMBAT_STATE::NONE;
	_oldPlayerState = _playerState;

	// 位置関連初期化
	_vMove = VGet(0, 0, 0);
	_vOldPos = VGet(0, 0, 0);
}

PlayerBase::~PlayerBase()
{
	// シールドシステムの終了処理
	if(_shieldSystem)
	{
		_shieldSystem->Terminate();
		_shieldSystem.reset();
	}
}

bool PlayerBase::Initialize()
{
	CharaBase::Initialize();	// 基底クラスの初期化

	_playerConfig = GetPlayerConfig();	// プレイヤー設定取得
	_playerAnim = GetPlayerAnimation();	// プレイヤーアニメーション名データ取得
	_renderConfig = GetRenderConfig();	// 表示設定データ取得

	// 共通初期化
	InitializePlayerConfig(_playerConfig);

	// 攻撃データの初期化
	InitializeAttackData();

	// 回避データの初期化
	InitializeDodgeData();

	// シールドシステム初期化
	//InitializeShieldData();

	// 状態初期化
	InitializeState();

	// アニメーション初期化
	InitializeAnimation();

	return true;
}

// 共通初期化
void PlayerBase::InitializePlayerConfig(PlayerConfig& config)
{
	// モデル読み込み
	int modelHandle = ResourceServer::GetInstance()->GetHandle(config.modelName);
	_animManager.SetModelHandle(modelHandle);

	// 位置の初期化
	_vPos = config.startPos;
	_vDir = VGet(0, 0, -1);

	_fMoveSpeed = 0.0f;			// 移動速度
	_fDirSpeed = 0.0f;			// 回転速度
	_fLife = config.life;		// 体力
	_fMaxLife = config.maxLife;	// 最大体力

	// アクション関係変数の初期化
	_fVelY = 0.0f;				// Y方向の速度
	_bIsJumping = false;		// ジャンプ中かどうか
	_bIsStanding = true;		// 着地しているかどうか
	_bIsCrouching = false;		// しゃがんでいるかどうか
	_bIsStartCrouch = false;	// しゃがみ開始フラグ

	// カプセルコリジョンの設定
	_vCollisionTop = VGet(0.0f, 0.0f, 0.0f);	// 上端
	_vCollisionBottom = VGet(0.0f, 0.0f, 0.0f);	// 下端
	_fCollisionR = 20.0f;						// 半径
	_fCollisionHeight = 60.0f;					// 高さ

	// 表示用オフセット
	_iDrawSizeOffset = config.drawSizeOffset;	// ずらす大きさ
	_iDrawOffsetX = config.drawOffsetX;
	_iDrawOffsetY = config.drawOffsetY;

	// 攻撃システム
	_bCanCombo = false;
	_iComboCount = 0;

	// 被弾設定の初期化
	_vHitDir = VGet(0, 0, 0);	// 被弾方向
	_fHitSpeed = 0.0f;			// 被弾速度
	_fHitSpeedDecay = 0.0f;		// 被弾速度減衰率
	_fHitTime = 0.0f;			// 被弾時間

	// 死亡関係
	_bIsAlive = true;				// 生きているから
	_bIsDeathAnimComplete = false;	// 死亡アニメーションが再生し終わったか
}

// 被弾設定初期化
void PlayerBase::InitializeHitConfig(const VECTOR& attackDirection)
{
	_vHitDir = VNorm(attackDirection);		// 被弾方向
	_fHitSpeed = HitConfig::HIT_SPEED;		// 被弾速度
	_fHitSpeedDecay = HitConfig::HIT_DECAY;	// 被弾速度減衰率
	_fHitTime = HitConfig::HIT_TIME;		// 被弾時間
}

// シールドデータ初期化
void PlayerBase::InitializeShieldData()
{
	// シールドシステム作成
	_shieldSystem = std::make_shared<PlayerShieldSystem>();

	// シールド所有者設定
	_shieldSystem->SetOwner(shared_from_this());
	
	// プレイヤー固有の設定を取得してシールドシステムに設定
	ShieldConfig config = GetShieldConfig();
	_shieldSystem->SetShieldConfig(config);

	// 初期化
	_shieldSystem->Initialize();
}

// 状態初期化
void PlayerBase::InitializeState()
{
	// 基礎ステータスの初期化
	_playerState.movementState = PLAYER_MOVEMENT_STATE::WAIT;	// 待機状態
	_playerState.attackState   = PLAYER_ATTACK_STATE::NONE;
	_playerState.shootState	   = PLAYER_SHOOT_STATE::NONE;
	_playerState.combatState   = PLAYER_COMBAT_STATE::NONE;

	// 古いステートを異なる値に設定して状態変化を認識させる
	_oldPlayerState.movementState = PLAYER_MOVEMENT_STATE::NONE;
	_oldPlayerState.attackState   = PLAYER_ATTACK_STATE::NONE;
	_oldPlayerState.shootState	  = PLAYER_SHOOT_STATE::NONE;
	_oldPlayerState.combatState   = PLAYER_COMBAT_STATE::NONE;
}

// アニメーション初期化
void PlayerBase::InitializeAnimation()
{
	AnimManager* animManager = GetAnimManager();

	const char* waitAnimName = _playerAnim.movement.wait;
	if(waitAnimName != nullptr)
	{
		// 待機アニメーションを設定
		animManager->ChangeAnimationByName(waitAnimName, 0.0f, 0);
	}
}

bool PlayerBase::Terminate()
{
	return true;
}

bool PlayerBase::Process()
{
	// 死亡処理
	CallDeath();

	// 共通処理呼び出し
	CallProcess();

	// 攻撃関係Process呼び出し用関数
	CallProcessAttack();

	// 弾発射処理の仮想関数
	ProcessShoot();

	// 回避関係Process呼び出し用関数
	CallProcessDodge();

	// シールド関係Process呼び出し用関数
	//CallProcessShield();

	// 親クラスの更新処理呼び出し
	CharaBase::Process();

	return true;
}

bool PlayerBase::Render()
{
	// 親クラスの描画処理呼び出し
	CharaBase::Render();

	return true;
}

// 被ダメージ処理
void PlayerBase::ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo)
{
	// 親クラスの被ダメージ処理呼び出し
	CharaBase::ApplyDamage(fDamage, eType, attackInfo);

	// 被弾状態に変更
	_playerState.combatState = PLAYER_COMBAT_STATE::HIT;

	// 攻撃方向を使って被弾設定初期化
	InitializeHitConfig(attackInfo.attackDir);
}

// 弾での被ダメージ処理
void PlayerBase::ApplyDamageByBullet(float fDamage, CHARA_TYPE chara)
{
	// 親クラスの被ダメージ処理呼び出し
	CharaBase::ApplyDamageByBullet(fDamage, chara);

	// 被弾状態に変更
	_playerState.combatState = PLAYER_COMBAT_STATE::HIT;
}

// カメラ角度に合わせて移動方向を変換する
VECTOR PlayerBase::TransformMoveDirection(VECTOR move, float cameraAngle)
{
	if(VSize(move) == 0.0f){ return move; }	// 移動量が0ならそのまま返す

	// カメラ角度に基づいて移動方向を変換
	float _cosAngle = cosf(cameraAngle);
	float _sinAngle = sinf(cameraAngle);

	VECTOR transformMove;

	// X軸とZ軸の変換
	transformMove.x = move.x * _cosAngle - move.z * _sinAngle;	// X軸方向
	transformMove.y = move.y;									// Y軸方向はそのまま
	transformMove.z = move.x * _sinAngle + move.z * _cosAngle;	// Z軸方向

	return transformMove;
}
