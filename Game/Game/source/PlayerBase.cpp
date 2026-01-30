// 担当 : 成田

#include "PlayerBase.h"

namespace
	{
	const float GRAVITY = 0.6f;	// 重力定数
}

PlayerBase::PlayerBase()
{
	// キャラの状態初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_eOldPlayerStatus = PLAYER_STATUS::NONE;
	_vMove = VGet(0, 0, 0);
	_vOldPos = VGet(0, 0, 0);




	//// アクション関係変数の初期化
	//_fVelY = 0.0f;
	//_bIsJumping = false;
	//_bIsStanding = true;
	//_bIsCrouching = false;
	//_bIsStartCrouch = false;

	//// 表示用オフセット
	//_iDrawSizeOffset = 0;
	//_iDrawOffsetX = 0;
	//_iDrawOffsetY = 0;

	//// 攻撃システム初期化
	//_vAttackColTop = VGet(0.0f, 0.0f, 0.0f);
	//_vAttackColBottom = VGet(0.0f, 0.0f, 0.0f);
	//_fAttackColR = 0.0f;
	//_bCanCombo = false;
	//_iComboCount = 0;

	//// カメラ角度初期化
	//_cameraAngle = 0.0f;
}

PlayerBase::~PlayerBase()
{

}

bool PlayerBase::Initialize()
{
	_playerConfig = GetPlayerConfig();
	_playerAnim = GetPlayerAnimation();
	_renderConfig = GetRenderConfig();     

	// 共通初期化
	InitializePlayerConfig(_playerConfig);

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

	// 基礎ステータスの初期化
	_ePlayerStatus = PLAYER_STATUS::NONE;
	_fMoveSpeed = 0.0f;						// 移動速度
	_fDirSpeed = 0.0f;						// 回転速度
	_fLife = config.life;					// 体力
	_fMaxLife = config.maxLife;				// 最大体力
	_fGravity = GRAVITY;					// 重力

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

	// 表示用オフセット
	_iDrawSizeOffset = config.drawSizeOffset;	// ずらす大きさ
	_iDrawOffsetX = config.drawOffsetX;
	_iDrawOffsetY = config.drawOffsetY;

	// 攻撃システム
	_bCanCombo = false;
	_iComboCount = 0;

	// 攻撃データの初期化
	InitializeAttackData();
}

bool PlayerBase::Terminate()
{
	return true;
}

bool PlayerBase::Process()
{
	// 死亡処理
	ProcessDeath();

	// プレイヤーが死亡していたら処理終了
	if(_ePlayerStatus == PLAYER_STATUS::DEATH){ return true; }

	// 共通処理呼び出し
	CallProcess();

	// 攻撃関係Process呼び出し用関数
	CallProcessAttack();

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
