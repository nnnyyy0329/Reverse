// 担当 : 成田

#pragma once
#include "CharaBase.h"
#include "AttackBase.h"
#include "DodgeSystem.h"
//#include "ShieldBase.h"
#include "PlayerShieldSystem.h"

// 前方宣言
class CameraManager;

// プレイヤー設定データ構造体
struct PlayerConfig
{
	// 移動速度設定
	float crouchMoveSpeed;	// しゃがみ移動速度
	float normalMoveSpeed;	// 通常移動速度
	float dashMoveSpeed;	// ダッシュ移動速度

	// 基礎ステータス
	float life;			// 体力
	float maxLife;		// 最大体力
	VECTOR startPos;	// 開始位置

	// 表示設定
	int drawSizeOffset;	// 描画サイズオフセット
	int drawOffsetX;	// 描画Xオフセット
	int drawOffsetY;	// 描画Yオフセット

	// リソース名
	std::string modelName;	// モデル名
};

// 表示設定構造体
struct RenderConfig
{
	const char* playerName;	// プレイヤー名
	COLOR_U8 debugColor;	// デバッグ描画色
};

// 攻撃定数構造体
struct AttackConstants
{
	float attackOffsetScale;	// 攻撃判定オフセット倍率
	int surfaceMaxComboCount;	// 表プレイヤー用コンボカウント
	int interiorMaxComboCount;	// 裏プレイヤー用コンボカウント
};

// 攻撃設定データ構造体
struct AttackConfig
{
	VECTOR topOffset;			// コリジョン上部位置
	VECTOR bottomOffset;		// コリジョン下部位置
	float radius;				// 半径
	float delay;				// 発生
	float duration;				// 持続
	float recovery;				// 硬直
	float damage;				// ダメージ
	ATTACK_STATE attackState;	// 攻撃状態
	float attackMoveSpeed;		// 攻撃中の移動速度
	std::string effectName;		// エフェクト名
	VECTOR effectOffset;		// エフェクト位置オフセット
	std::string soundName;		// サウンド名
};

// 基本移動アニメーション構造体
struct PlayerMovementAnimations
{
	const char* wait;        // 待機
	const char* walk;        // 歩行
	const char* run;         // 走行
	const char* jumpUp;      // ジャンプ（上昇）
	const char* jumpDown;    // ジャンプ（下降）
	const char* crouchWait;  // しゃがみ待機
	const char* crouchWalk;  // しゃがみ歩行
};

// 攻撃アニメーション構造体
struct PlayerAttackAnimations
{
	const char* firstAttack;   // 1段目攻撃
	const char* secondAttack;  // 2段目攻撃
	const char* thirdAttack;   // 3段目攻撃
	const char* fourthAttack;  // 4段目攻撃
	const char* fifthAttack;   // 5段目攻撃
};

// 発射アニメーション構造体
struct PlayerShootAnimations
{
	const char* shootReady;     // 発射構え
	const char* rightArmShoot;  // 右腕発射
	const char* leftArmShoot;   // 左腕発射
	const char* shootMove;      // 発射移動
};

// 戦闘アニメーション構造体
struct PlayerCombatAnimations
{
	const char* transform;		// 変身
	const char* transCancel;	// 変身解除
	const char* guard;			// ガード
	const char* hit;			// 被弾
	const char* dodge;			// 回避
	const char* death;			// 死亡
};

// 統合アニメーション構造体
struct PlayerAnimations
{
	PlayerMovementAnimations movement;
	PlayerAttackAnimations attack;
	PlayerShootAnimations shoot;
	PlayerCombatAnimations combat;
};

// 基本移動状態
enum class PLAYER_MOVEMENT_STATE
{
	NONE,
	WAIT,			// 待機
	WALK,			// 歩行
	RUN,			// 走行
	JUMP_UP,		// ジャンプ（上昇）
	JUMP_DOWN,		// ジャンプ（下降）
	CROUCH_WAIT,	// しゃがみ待機
	CROUCH_WALK,	// しゃがみ歩行
	_EOT_,
};

// 攻撃状態
enum class PLAYER_ATTACK_STATE
{
	NONE,
	FIRST_ATTACK,	// 1段目攻撃
	SECOND_ATTACK,	// 2段目攻撃
	THIRD_ATTACK,	// 3段目攻撃
	FOURTH_ATTACK,	// 4段目攻撃
	FIFTH_ATTACK,	// 5段目攻撃
	_EOT_,
};

// 弾の発射状態
enum class PLAYER_SHOOT_STATE
{
	NONE,
	SHOOT_READY,		// 発射構え
	RIGHT_ARM_SHOOT,	// 右腕発射
	LEFT_ARM_SHOOT,		// 左腕発射
	SHOOT_MOVE,			// 発射移動
	_EOT_,
};

// 特殊状態
enum class PLAYER_COMBAT_STATE
{
	NONE,
	TRANSFORM,		// 変身
	TRANS_CANCEL,	// 変身解除
	GUARD,			// ガード
	HIT,			// 被弾
	DODGE,			// 回避
	DEATH,			// 死亡
	_EOT_,
};

// 統合状態管理構造体
struct PlayerState
{
	PLAYER_MOVEMENT_STATE movementState;	// 基本移動状態
	PLAYER_ATTACK_STATE attackState;		// 攻撃状態
	PLAYER_SHOOT_STATE shootState;			// 弾発射状態
	PLAYER_COMBAT_STATE combatState;		// 特殊状態

	bool IsStateAttacking() const { return attackState   != PLAYER_ATTACK_STATE::NONE; }
	bool IsStateMoving()	const { return movementState != PLAYER_MOVEMENT_STATE::NONE; }
	bool IsStateShooting()	const { return shootState	 != PLAYER_SHOOT_STATE::NONE; }
	bool IsStateCombat()	const { return combatState	 != PLAYER_COMBAT_STATE::NONE; }
};

class PlayerBase : public CharaBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	// 被ダメージ処理
	virtual void ApplyDamage
	(
		float fDamage,
		ATTACK_OWNER_TYPE eType,
		const ATTACK_COLLISION& attackInfo
	);	

	// 共通初期化
	void InitializePlayerConfig(PlayerConfig& config);			// プレイヤー設定初期化
	void InitializeHitConfig(const VECTOR& attackDirection);	// 被弾設定初期化
	void InitializeAttackData();								// 攻撃データ初期化
	void InitializeDodgeData();									// 回避データ初期化
	void InitializeShieldData();								// シールドデータ初期化
	void InitializeState();										// 状態初期化
	void InitializeAnimation();									// アニメーション初期化

	// 共通処理
	void CallProcess();				// Process呼び出し用関数
	void ProcessMovePlayer();		// 移動処理
	void ProcessInputMove();		// 移動入力処理
	void ProcessCollisionPos();		// コリジョン位置更新処理
	void ProcessStatusAnimation();	// 状態別アニメーション処理
	void ProcessPlayAnimation();	// アニメーション処理
	void ProcessHit();				// 被弾処理
	void ProcessDebug();			// デバッグ処理
	bool IsHitStop();				// 被弾硬直中かチェック

	// デバッグ描画共通
	void DebugRender();				// デバッグ情報描画
	void DrawBaseData();			// 基本データ表示
	void DrawCoordinate();			// 座標表示
	void DrawStatus	();				// ステータス表示
	void DrawParameter();			// パラメーター表示
	void DrawColPos	();				// コリジョン位置表示

	// ステータス文字列変換
	std::string GetCurrentStateString() const;								// 現在の状態文字列取得
	std::string GetMovementStateString(PLAYER_MOVEMENT_STATE state) const;	// 基本移動状態文字列取得
	std::string GetAttackStateString(PLAYER_ATTACK_STATE state) const;		// 攻撃状態文字列取得
	std::string GetShootStateString(PLAYER_SHOOT_STATE state) const;		// 弾発射状態文字列取得
	std::string GetCombatStateString(PLAYER_COMBAT_STATE state) const;		// 特殊状態文字列取得

	// 現在のアニメーション名取得
	const char* GetCurrentAnimationName() const;

	// ループカウント取得関数
	int GetLoopCount() const;

	// 状態変化チェック
	bool HasStateChanged()const;

	// 各プレイヤー固有の設定を取得
	virtual PlayerConfig GetPlayerConfig() = 0;			// プレイヤー設定を取得	
	virtual PlayerAnimations GetPlayerAnimation() = 0;	// プレイヤーアニメーション名データを取得
	virtual RenderConfig GetRenderConfig() = 0;			// 表示設定データを取得

	void SetCameraAngle(float cameraAngle) { _cameraAngle = cameraAngle; }	// カメラ角度設定
	VECTOR TransformMoveDirection(VECTOR move, float cameraAngle);			// カメラ角度に合わせて移動方向を変換する	

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}
	int GetInput()const{ return _key; }

	/*****ゲッターセッター*****/
	VECTOR GetAttackColTop(){ return _vAttackColTop; }			// 攻撃コリジョン上部
	VECTOR GetAttackColBottom(){ return _vAttackColBottom; }	// 攻撃コリジョン下部
	float GetAttackColR(){ return _fAttackColR; }				// 攻撃コリジョン半径

	// システム設定
	void SetDodgeSystem(std::shared_ptr<DodgeSystem> dodgeSystem) { _dodgeSystem = dodgeSystem; }
	void SetCameraManager(std::shared_ptr<CameraManager> cameraManager) { _cameraManager = cameraManager; }

	// ステート
	PLAYER_MOVEMENT_STATE GetMovementState()const{ return _playerState.movementState; }			// 基本移動状態取得
	void SetMovementState(PLAYER_MOVEMENT_STATE state){ _playerState.movementState = state; }	// 基本移動状態設定

	PLAYER_ATTACK_STATE GetAttackState()const{ return _playerState.attackState; }		// 攻撃状態取得
	void SetAttackState(PLAYER_ATTACK_STATE state){ _playerState.attackState = state; }	// 攻撃状態設定

	PLAYER_SHOOT_STATE GetShootState()const{ return _playerState.shootState; }			// 弾発射状態取得
	void SetShootState(PLAYER_SHOOT_STATE state){ _playerState.shootState = state; }	// 弾発射状態設定

	PLAYER_COMBAT_STATE GetCombatState()const{ return _playerState.combatState; }		// 特殊状態取得
	void SetCombatState(PLAYER_COMBAT_STATE state){ _playerState.combatState = state; }	// 特殊状態設定
	
protected:	// 攻撃関係

	virtual AttackConstants GetAttackConstants()const = 0;		// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[]) = 0;	// 攻撃設定を取得

	// 攻撃システム
	std::vector<std::shared_ptr<AttackBase>> _attacks;	// 攻撃配列
	std::vector<PLAYER_ATTACK_STATE> _attackStatuses;	// 攻撃状態配列

	// PlayerBase_Attack.cppで定義
	void CallProcessAttack();		// 攻撃関係Process呼び出し用関数
	void ProcessAttackColPos();		// コリジョン位置の更新処理
	void ProcessAttack();			// 攻撃処理
	void ProcessBranchAttack();		// 攻撃分岐処理
	void ReceiveAttackColData();	// 攻撃コリジョンの情報受け取り関数
	bool IsStartAttack();			// 攻撃を開始できるかチェック
	bool CanNextAttack();			// 次の攻撃が可能かチェック
	bool IsAttacking();				// 攻撃中かチェック

	void UpdateAttackColPos(std::shared_ptr<AttackBase> attack, VECTOR& topOffset, VECTOR& bottomOffset, VECTOR& baseOffset);	// 攻撃判定の位置更新処理
	void ProcessStartAttack(int comboCount, PLAYER_ATTACK_STATE nextStatus, std::shared_ptr<AttackBase> attack);				// 攻撃開始処理
	void ProcessAttackReaction(int attackIndex);									// 攻撃反応処理
	void ProcessAttackEffect(int attackIndex, std::vector<AttackConfig> configs);	// 攻撃エフェクト処理
	void ProcessAttackSound(int attackIndex, std::vector<AttackConfig> configs);	// 攻撃サウンド処理
	void ProcessComboAttack(int attackIndex);										// コンボ攻撃処理
	void ProcessAttackFinish(std::shared_ptr<AttackBase> attack);					// 攻撃終了処理
	void EndAttackSequence();														// 攻撃課程修了
	void ProcessNextAttack(int currentIndex);										// 次の攻撃処理
	std::shared_ptr<AttackBase> GetAttackByStatus(PLAYER_ATTACK_STATE status);		// 状態に対応する攻撃を取得
	int GetInstanceId();															// ID取得関数
	int GetAttackIndexByStatus(PLAYER_ATTACK_STATE status);							// 状態から攻撃インデックスを取得
	int GetMaxComboCount()const;													// 最大コンボ数取得

protected:	// 弾発射関係
	
	virtual void ProcessShoot(){};	// 発射処理の仮想関数

	virtual bool IsShooting()const{ return _playerState.shootState != PLAYER_SHOOT_STATE::NONE; }


protected:	// 回避関係

	std::shared_ptr<DodgeSystem> _dodgeSystem;		// 回避システム

	// PlayerBase_Dodge.cppで定義
	void CallProcessDodge();		// 回避関係Process呼び出し用関数
	void ProcessDodge();			// 回避処理
	void ProcessStartDodge();		// 回避開始処理
	void ProcessEndDodge();			// 回避終了処理
	bool CanDodge();				// 回避可能かチェック
	bool IsDodging();				// 回避中かチェック
	bool IsInvincible();			// 無敵状態かチェック

	// 各プレイヤー固有の回避設定を取得
	virtual DodgeConfig GetDodgeConfig() = 0;

protected: 	// シールド関係

	std::shared_ptr<PlayerShieldSystem> _shieldSystem;	// シールドシステム

	// PlayerBase_Shield.cpp で定義
	void CallProcessShield();      // シールド関係Process呼び出し用関数
	void ProcessShield();          // シールド処理
	void ProcessShieldInput();     // シールド入力処理
	bool IsShielding();            // シールド中かチェック

	// 各プレイヤー固有のシールド設定取得（純粋仮想関数）
	virtual ShieldConfig GetShieldConfig() = 0;

protected:	// 死亡関係
	void CallDeath();				// 死亡関係関係関数呼び出し
	void ProcessDeath();			// 死亡処理
	void CheckDeathAnimFinished();	// 死亡アニメーションが再生し終わったか
	void CheckDeath();				// 死亡したか

	bool IsDeath()const;			// 死亡したか
	bool IsAlive()const;			// プレイヤーが死亡したか
	bool IsStateDeath()const;		// 死亡ステートになったか

protected:

	std::shared_ptr<CameraManager> _cameraManager;	// カメラマネージャー

	// 共通の設定
	PlayerConfig		_playerConfig;	// プレイヤー設定データ
	PlayerAnimations	_playerAnim;	// プレイヤーアニメーション名データ
	RenderConfig		_renderConfig;	// 表示設定データ

	// 状態
	PlayerState _playerState;		// 現在の状態
	PlayerState _oldPlayerState;	// 前フレームの状態

	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;

	// アクション関係変数
	float _fVelY;			// Y方向の速度
	bool _bIsJumping;		// ジャンプ中かどうか
	bool _bIsStanding;		// 着地しているかどうか
	bool _bIsCrouching;		// しゃがんでいるかどうか
	bool _bIsStartCrouch;	// しゃがみ開始フラグ

	// 表示用オフセット
	int _iDrawSizeOffset;	// ずらす大きさ
	int _iDrawOffsetX;		
	int _iDrawOffsetY;

	// 攻撃コリジョン情報の受け取り用
	VECTOR _vAttackColTop;
	VECTOR _vAttackColBottom;
	float _fAttackColR;
	bool _bCanCombo;	// コンボ可能フラグ
	int _iComboCount;	// コンボカウント

	// 被弾情報
	VECTOR _vHitDir;		// 被弾方向
	float _fHitSpeed;		// 被弾速度
	float _fHitSpeedDecay;	// 被弾速度減衰
	float _fHitTime;		// 被弾時間

	// カメラ角度
	float _cameraAngle;

	// 死亡関係変数
	bool _bIsAlive;				// 生きているか
	bool _bIsDeath;				// 死亡したか
	bool _bIsDeathAnimComplete;	// 死亡アニメーションが再生し終わったか
};



class PlayerInput
{
public:

	// 入力状態の取得
	bool IsMoving() const;
	bool IsDashing() const;
	bool IsAttackPressed() const;
	bool IsDodgePressed() const;

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}

protected:
	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;
};

class PlayerMove
{
public:
	void InitializePlayerConfig(PlayerConfig& config);	// プレイヤー設定初期化
	void ProcessMovement(const PlayerInput& input, float cameraAngle);

protected:
	PlayerConfig _config;
};

class PlayerAnimator
{

};

//class PlayerDraw
//{
//
//};
//
//class PlayerAttackSystem
//{
//
//};
//
//class PlayerDodgeSystem
//{
//
//};

