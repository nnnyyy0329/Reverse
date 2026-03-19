// 担当 : 成田

#pragma once
#include "CharaBase.h"
#include "AttackBase.h"
//#include "ShieldBase.h"
#include "DodgeSystem.h"
#include "BulletManager.h"
#include "PlayerShieldSystem.h"
#include "PlayerAbsorbAttackSystem.h"

// 前方宣言
class CameraManager;
class AbsorbAttack;

/// @brief プレイヤーの基本移動状態列挙型
enum class PLAYER_MOVEMENT_STATE
{
	NONE,
	WAIT,			///< 待機
	WALK,			///< 歩行
	RUN,			///< 走行
	JUMP_UP,		///< ジャンプ（上昇）
	JUMP_DOWN,		///< ジャンプ（下降）
	CROUCH_WAIT,	///< しゃがみ待機
	CROUCH_WALK,	///< しゃがみ歩行
	_EOT_,
};

/// @brief プレイヤーの攻撃状態列挙型
enum class PLAYER_ATTACK_STATE
{
	NONE,
	FIRST_ATTACK,	///< 1段目攻撃
	SECOND_ATTACK,	///< 2段目攻撃
	THIRD_ATTACK,	///< 3段目攻撃
	FOURTH_ATTACK,	///< 4段目攻撃
	FIFTH_ATTACK,	///< 5段目攻撃
	AREA_ATTACK,	///< 範囲攻撃
	FIRST_SKILL,	///< スキル1
	SECOND_SKILL,	///< スキル2
	_EOT_,
};

/// @brief プレイヤーの弾発射状態列挙型
enum class PLAYER_SHOOT_STATE
{
	NONE,
	SHOOT_READY,		///< 発射構え
	RIGHT_ARM_SHOOT,	///< 右腕発射
	LEFT_ARM_SHOOT,		///< 左腕発射
	SHOOT_MOVE,			///< 発射移動
	_EOT_,
};

/// @brief プレイヤーの吸収攻撃状態列挙型
enum class PLAYER_ABSORB_STATE
{
	NONE,
	ABSORB_READY,	///< 吸収構え
	ABSORB_ACTIVE,	///< 吸収中
	ABSORB_END,		///< 吸収終了
	_EOT_,
};

/// @brief プレイヤーの特殊状態列挙型
enum class PLAYER_COMBAT_STATE
{
	NONE,
	TRANSFORM,		///< 変身
	TRANS_CANCEL,	///< 変身解除
	GUARD,			///< ガード
	HIT,			///< 被弾
	DODGE,			///< 回避
	DEATH,			///< 死亡
	_EOT_,
};

/// @brief プレイヤーの基本設定をまとめた構造体
struct PlayerConfig
{
	// 移動速度設定
	float crouchMoveSpeed;	// しゃがみ移動速度
	float normalMoveSpeed;	// 通常移動速度
	float dashMoveSpeed;	// ダッシュ移動速度

	// 基礎ステータス
	float life;			// 体力
	float maxLife;		// 最大体力

	// 表示設定
	int drawSizeOffset;	// 描画サイズオフセット
	int drawOffsetX;	// 描画Xオフセット
	int drawOffsetY;	// 描画Yオフセット

	// リソース名
	std::string modelName;	// モデル名
};

/// @brief プレイヤーの表示設定をまとめた構造体
struct RenderConfig
{
	const char* playerName;	// プレイヤー名
	COLOR_U8 debugColor;	// デバッグ描画色
};

/// @brief 攻撃に関する定数をまとめた構造体
struct AttackConstants
{
	int surfaceMaxComboCount;	// 表プレイヤー用コンボカウント
	int interiorMaxComboCount;	// 裏プレイヤー用コンボカウント
};

/// @brief 攻撃の向き調整に関する設定をまとめた構造体
struct AttackDirAdjustConfig
{
	bool canDirAdjust;	// 向き調整が可能かどうか
};

/// @brief 攻撃時にどちらの腕で攻撃するかの設定をまとめた構造体
struct AttackArmConfig
{
	int useFromBody;		// どの部位で再生するか(2 : 腕以外、1 : 右腕、0 : 左腕、-1 : 再生しない)
	int rightArmFrameIndex;	// 右腕攻撃のフレームインデックス
	int leftArmFrameIndex;	// 左腕攻撃のフレームインデックス
};

/// @brief 範囲攻撃の設定をまとめた構造体
struct AreaAttackConfig
{
	VECTOR centerOffset;	// コリジョン中心位置オフセット
	float radius;			// 半径
	float height;			// 高さ
	float delay;			// 発生
	float duration;			// 持続
	float recovery;			// 硬直
	float damage;			// ダメージ
	bool isHit;				// ヒットフラグ
};

/// @brief プレイヤーの基本移動アニメーションをまとめた構造体
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

/// @brief プレイヤーの攻撃アニメーションをまとめた構造体
struct PlayerAttackAnimations
{
	const char* firstAttack;	// 1段目攻撃
	const char* secondAttack;	// 2段目攻撃
	const char* thirdAttack;	// 3段目攻撃
	const char* fourthAttack;	// 4段目攻撃
	const char* fifthAttack;	// 5段目攻撃
	const char* areaAttack;		// 範囲攻撃
	const char* firstSkill;		// スキル1
	const char* secondSkill;	// スキル2
};

/// @brief プレイヤーの弾発射アニメーションをまとめた構造体
struct PlayerShootAnimations
{
	const char* shootReady;     // 発射構え
	const char* rightArmShoot;  // 右腕発射
	const char* leftArmShoot;   // 左腕発射
	const char* shootMove;      // 発射移動
};

/// @brief プレイヤーの吸収攻撃アニメーションをまとめた構造体
struct PlayerAbsorbAnimations
{
	const char* absorbReady;	// 吸収構え
	const char* absorbActive;	// 吸収中
	const char* absorbEnd;		// 吸収終了
};

/// @brief プレイヤーの特殊アニメーションをまとめた構造体
struct PlayerCombatAnimations
{
	const char* transform;		// 変身
	const char* transCancel;	// 変身解除
	const char* guard;			// ガード
	const char* hit;			// 被弾
	const char* dodge;			// 回避
	const char* death;			// 死亡
};

/// @brief プレイヤーのアニメーションをまとめた構造体
struct PlayerAnimations
{
	PlayerMovementAnimations	movement;	// 基本行動
	PlayerAttackAnimations		attack;		// 攻撃
	PlayerShootAnimations		shoot;		// 弾発射
	PlayerAbsorbAnimations		absorb;		// 吸収攻撃
	PlayerCombatAnimations		combat;		// 特殊
};

/// @brief プレイヤーの状態をまとめた構造体
struct PlayerState
{
	PLAYER_MOVEMENT_STATE	movementState;	// 基本移動状態
	PLAYER_ATTACK_STATE		attackState;	// 攻撃状態
	PLAYER_SHOOT_STATE		shootState;		// 弾発射状態
	PLAYER_ABSORB_STATE		absorbState;	// 吸収攻撃状態
	PLAYER_COMBAT_STATE		combatState;	// 特殊状態

	// 状態チェック関数
	bool IsStateAttacking() const { return attackState	 != PLAYER_ATTACK_STATE::NONE; }	// 攻撃状態かどうか
	bool IsStateMoving()	const { return movementState != PLAYER_MOVEMENT_STATE::NONE; }	// 移動状態かどうか
	bool IsStateShooting()	const { return shootState	 != PLAYER_SHOOT_STATE::NONE; }		// 発射状態かどうか
	bool IsStateAbsorbing()	const { return absorbState	 != PLAYER_ABSORB_STATE::NONE; }	// 吸収攻撃状態かどうか
	bool IsStateCombat()	const { return combatState	 != PLAYER_COMBAT_STATE::NONE; }	// 特殊状態かどうか

	// 特定の状態かどうかをチェックする関数
	bool IsInMovementState(PLAYER_MOVEMENT_STATE state)	const { return movementState == state; }	// 特定の移動状態かチェック
	bool IsInAttackState(PLAYER_ATTACK_STATE state)		const { return attackState == state; }		// 特定の攻撃状態かチェック
	bool IsInShootState(PLAYER_SHOOT_STATE state)		const { return shootState == state; }		// 特定の発射状態かチェック
	bool IsInAbsorbState(PLAYER_ABSORB_STATE state)		const { return absorbState == state; }		// 特定の吸収状態かチェック
	bool IsInCombatState(PLAYER_COMBAT_STATE state)		const { return combatState == state; }		// 特定の特殊状態かチェック

	// 状態リセット関数
	void StateReset()
	{
		movementState = PLAYER_MOVEMENT_STATE::NONE;
		attackState = PLAYER_ATTACK_STATE::NONE;
		shootState = PLAYER_SHOOT_STATE::NONE;
		absorbState = PLAYER_ABSORB_STATE::NONE;
		combatState = PLAYER_COMBAT_STATE::NONE;
	}
};

/// @brief プレイヤーの基底クラス
class PlayerBase : public CharaBase
{
public:
	PlayerBase();
	virtual ~PlayerBase();

	virtual bool	Initialize();	// 初期化
	virtual bool	Terminate();	// 終了
	virtual bool	Process();		// 更新
	virtual bool	Render();		// 描画

	/// @brief 被ダメージ処理関数
	///
	/// @param fDamage ダメージ量
	/// @param eType 攻撃の所有者タイプ
	/// @param attackInfo 攻撃コリジョン情報
	virtual void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const AttackCollision& attackInfo)override;	

	/// @brief 弾による被ダメージ処理関数
	/// 
	/// @param fDamage ダメージ量
	/// @param chara 弾のキャラタイプ
	virtual void ApplyDamageByBullet(float fDamage, CHARA_TYPE chara, BULLET_TYPE bulletType)override;										

	// 共通初期化
	void InitializePlayerConfig(PlayerConfig& config);			// プレイヤー設定初期化
	void InitializeHitConfig(const VECTOR& attackDirection);	// 被弾設定初期化
	void InitializeDodgeData();									// 回避データ初期化
	void InitializeShieldData();								// シールドデータ初期化
	void InitializeState();										// 状態初期化
	void InitializeAnimation();									// アニメーション初期化

	// 共通処理
	void CallProcess();						// Process呼び出し用関数
	void ProcessMovePlayer();				// 移動処理
	void ProcessInputMove();				// 移動入力処理
	void ProcessCollisionPos();				// コリジョン位置更新処理
	void ProcessStatusAnimation();			// 状態別アニメーション処理
	void ProcessPlayAnimation();			// アニメーション処理
	void ProcessReturnNormalMotion();		// 通常モーションに戻す処理
	void ProcessHit();						// 被弾処理
	void ProcessDebug();					// デバッグ処理
	bool IsHitStop();						// 被弾硬直中かチェック
	bool IsAnimationFinishedConst()const;	// アニメーションが終了したかの処理

	// デバッグ描画共通
	void DebugRender();				// デバッグ情報描画
	void DrawBaseData();			// 基本データ表示
	void DrawCoordinate();			// 座標表示
	void DrawDirection();			// 向き表示
	void DrawStatus();				// ステータス表示
	void DrawParameter();			// パラメーター表示
	void DrawColPos();				// コリジョン位置表示

	// ステータス文字列変換
	std::string GetCurrentStateString() const;								// 現在の状態文字列取得
	std::string GetMovementStateString(PLAYER_MOVEMENT_STATE state)	const;	// 基本移動の状態文字列取得
	std::string GetAttackStateString(PLAYER_ATTACK_STATE state)	const;		// 攻撃状態の文字列取得
	std::string GetShootStateString(PLAYER_SHOOT_STATE state)	const;		// 弾発射状態の文字列取得
	std::string GetAbsorbStateString(PLAYER_ABSORB_STATE state)	const;		// 吸収攻撃状態の文字列取得
	std::string GetCombatStateString(PLAYER_COMBAT_STATE state)	const;		// 特殊状態の文字列取得

	// 現在のアニメーション名取得
	const char* GetCurrentAnimationName() const;

	// ループカウント取得関数
	int GetLoopCount() const;

	// 状態変化チェック
	bool HasStateChanged()const;

	// 各プレイヤー固有の設定を取得
	virtual PlayerConfig GetPlayerConfig() = 0;											// プレイヤー設定を取得	
	virtual PlayerAnimations GetPlayerAnimation() = 0;									// プレイヤーアニメーション名データを取得
	virtual RenderConfig GetRenderConfig() = 0;											// 表示設定データを取得

	void SetCameraAngle(float cameraAngle) { _cameraAngle = cameraAngle; }	// カメラ角度設定
	VECTOR TransformMoveDirection(VECTOR move, float cameraAngle);			// カメラ角度に合わせて移動方向を変換する	


	/*****ゲッターセッター*****/

	// 攻撃コリジョン情報
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

	PLAYER_ABSORB_STATE GetAbsorbState()const{ return _playerState.absorbState; }		// 吸収攻撃状態取得
	void SetAbsorbState(PLAYER_ABSORB_STATE state){ _playerState.absorbState = state; }	// 吸収攻撃状態設定

	PLAYER_COMBAT_STATE GetCombatState()const{ return _playerState.combatState; }		// 特殊状態取得
	void SetCombatState(PLAYER_COMBAT_STATE state){ _playerState.combatState = state; }	// 特殊状態設定


	// 吸収攻撃関係 --- 今後クラスで分ける予定 ------------------------------------------------------
	/// @brief 吸収攻撃システムの取得関数
	///
	/// @return 吸収攻撃システムのポインタ(デフォルトはnullptr)
	virtual PlayerAbsorbAttackSystem* GetAbsorbAttackSystem(){ return nullptr; };



	// 状態リセット
	void SetStateReset(){ return _playerState.StateReset(); }

	// 移動ステートか
	bool IsMoving() { return _playerState.IsStateMoving(); }


	// プレイヤーの動作関連コンポーネントクラスを実装予定
	// それぞれのシステムはユニークポインタで管理して実装する

protected:	// 攻撃関係 --- 今後クラスで分ける予定 ------------------------------------------------------

	virtual AttackConstants GetAttackConstants()const{ return AttackConstants{}; };	// 攻撃定数を取得
	virtual void GetAttackColConfigs(AttackCollision configs[]){};					// 攻撃設定を取得
	virtual void GetAttackColOffsetConfigs(AttackColOffset configs[]){};			// 攻撃コリジョンオフセット設定を取得
	virtual void GetAttackDirAdjustConfigs(AttackDirAdjustConfig configs[]){};		// 攻撃向き調整設定を取得
	virtual AreaAttackConfig GetAreaAttackConfigs(){ return AreaAttackConfig{}; };	// 範囲攻撃設定を取得
	virtual void GetAttackEffectConfigs(AttackEffectConfig configs[]){};				// 演出設定を取得
	virtual void GetAttackArmConfigs(AttackArmConfig config[]){};					// 攻撃の腕設定を取得

	// 攻撃システム
	std::vector<std::shared_ptr<AttackBase>> _attacks;	// 攻撃配列
	std::vector<PLAYER_ATTACK_STATE> _attackStatuses;	// 攻撃状態配列

	// 攻撃関連の情報設定関数
	void InitializeAttackData();				// 攻撃データ初期化
	void InitializeAttackConfigs(int maxCombo);	// 攻撃設定配列初期化
	void SetAttackStatusData(int maxCombo);		// 攻撃状態を攻撃配列に入れる
	void CreateAttackData(int maxCombo);		// 攻撃コリジョンデータ作成	
	void SetAttackColData(AttackCollision config, std::shared_ptr<AttackBase> attack);			// 攻撃コリジョン情報設定
	void SetAttackOffsetData(AttackColOffset config, std::shared_ptr<AttackBase> attack);		// 攻撃オフセット情報設定
	void SetCanDirAdjustData(AttackDirAdjustConfig config, std::shared_ptr<AttackBase> attack);	// 攻撃向き調整情報設定
	void SetAttackEffectData(AttackEffectConfig config, std::shared_ptr<AttackBase> attack);	// 攻撃エフェクト情報設定
	void SetAttackArmData(AttackArmConfig config, std::shared_ptr<AttackBase> attack);			// 攻撃の腕設定情報設定

	// PlayerBase_Attack.cppで定義
	void CallProcessAttack();		// 攻撃関係Process呼び出し用関数
	//void ProcessAttackColPos();		// コリジョン位置の更新処理
	void ProcessAttack();			// 攻撃処理
	void ProcessBranchAttack();		// 攻撃分岐処理
	void ReceiveAttackColData();	// 攻撃コリジョンの情報受け取り関数
	bool CanStartAttack();			// 攻撃を開始できるかチェック
	bool CanNextAttack();			// 次の攻撃が可能かチェック
	bool IsAttacking();				// 攻撃中かチェック
	bool IsAttackInput();			// 攻撃入力があるかチェック

	void ProcessStartAttack(int comboCount, PLAYER_ATTACK_STATE nextStatus, std::shared_ptr<AttackBase> attack);	// 攻撃開始処理
	void ProcessAttackReaction(int attackIndex, std::shared_ptr<AttackBase> attack);	// 攻撃反応処理
	void ProcessAttackRegister(std::shared_ptr<AttackBase> attack);						// 攻撃登録処理
	void ProcessComboAttack(int attackIndex);											// コンボ攻撃処理
	void ProcessAttackFinish(std::shared_ptr<AttackBase> attack);						// 攻撃終了処理
	void EndAttackSequence();															// 攻撃課程修了
	void ProcessNextAttack(int currentIndex);											// 次の攻撃処理
	std::shared_ptr<AttackBase> GetAttackByStatus(PLAYER_ATTACK_STATE status);			// 状態に対応する攻撃を取得
	int GetInstanceId();																// ID取得関数
	int GetAttackIndexByStatus(PLAYER_ATTACK_STATE status);								// 状態から攻撃インデックスを取得
	int GetMaxComboCount()const;														// 最大コンボ数取得

	// 各攻撃の演出設定を保存
	std::vector<AttackEffectConfig> _attackEffectConfigs;  
	std::vector<AttackArmConfig> _attackArmConfigs;

	// 攻撃コリジョン情報の受け取り用
	VECTOR _vAttackColTop;
	VECTOR _vAttackColBottom;
	float _fAttackColR;
	bool _bCanCombo;	// コンボ可能フラグ
	int _iComboCount;	// コンボカウント

protected:	// 弾発射関係 --- 今後クラスで分ける予定 ------------------------------------------------------
	
	virtual BulletConfig GetBulletConfig(){ return BulletConfig{}; }					// 弾発射設定を取得
	virtual BulletEffectConfig GetBulletEffectConfig(){ return BulletEffectConfig{}; }	// 弾演出設定の取得

	virtual void ProcessShoot(){};	// 発射処理の仮想関数

protected:	// 吸収攻撃関係 --- 今後クラスで分ける予定 ------------------------------------------------------

	/// @brief 吸収攻撃処理の仮想関数
	virtual void ProcessAbsorb(){};	

protected:	// 回避関係 --- 今後クラスで分ける予定 ------------------------------------------------------

	std::shared_ptr<DodgeSystem> _dodgeSystem;		// 回避システム

	// PlayerBase_Dodge.cppで定義
	void CallProcessDodge();		// 回避関係Process呼び出し用関数
	void ProcessDodge();			// 回避処理
	void ProcessStartDodge();		// 回避開始処理
	void ProcessEndDodge();			// 回避終了処理
	bool CanStartDodge();			// 回避を開始できるかチェック
	bool CanDodge();				// 回避可能かチェック
	bool IsDodging();				// 回避中かチェック
	bool IsInvincible();			// 無敵状態かチェック

	// 各プレイヤー固有の回避設定を取得
	virtual DodgeConfig GetDodgeConfig() = 0;

protected: 	// シールド関係 --- 今後クラスで分ける予定 ------------------------------------------------------

	std::shared_ptr<PlayerShieldSystem> _shieldSystem;	// シールドシステム

	// PlayerBase_Shield.cpp で定義
	void CallProcessShield();      // シールド関係Process呼び出し用関数
	void ProcessShield();          // シールド処理
	void ProcessShieldInput();     // シールド入力処理
	bool IsShielding();            // シールド中かチェック

	// プレイヤー固有のシールド設定取得（仮想関数）
	virtual ShieldConfig GetShieldConfig(){ return ShieldConfig{}; }

protected:	// 死亡関係 --- 今後クラスで分ける予定 ------------------------------------------------------

	/// @brief 死亡処理呼び出し関数
	void CallDeath();				

	/// @brief 死亡処理
	void ProcessDeath();

	/// @brief 死亡カメラ演出開始処理(テスト)
	void StartDeathCameraEffect();

	/// @brief 死亡状態更新
	void UpdateDeathState();

	/// @brief 死亡後の処理
	void UpdateAfterDeath();

	/// @brief 死亡したかチェック
	bool IsDeath()const;

	/// @brief 生きているかチェック
	bool IsAlive()const;		

	/// @brief 死亡ステートかチェック
	bool IsStateDeath()const;	

	// 死亡関係変数
	bool _bIsAlive;				// 生きているか
	bool _bIsDeathAnimComplete;	// 死亡アニメーションが再生し終わったか

protected:

	std::shared_ptr<CameraManager> _cameraManager;	// カメラマネージャー

	// 共通の設定
	PlayerConfig		_playerConfig;	// プレイヤー設定データ
	PlayerAnimations	_playerAnim;	// プレイヤーアニメーション名データ
	RenderConfig		_renderConfig;	// 表示設定データ

	// 状態
	PlayerState			_playerState;		// 現在の状態
	PlayerState			_oldPlayerState;	// 前フレームの状態

	// アクション関係変数
	float	_fVelY;				// Y方向の速度
	bool	_bIsDashInput;		// ダッシュ入力があるかどうか
	bool	_bIsJumping;		// ジャンプ中かどうか
	bool	_bIsStanding;		// 着地しているかどうか
	bool	_bIsCrouching;		// しゃがんでいるかどうか
	bool	_bIsStartCrouch;	// しゃがみ開始フラグ

	// 表示用オフセット
	int _iDrawSizeOffset;	// ずらす大きさ
	int _iDrawOffsetX;		
	int _iDrawOffsetY;

	// 被弾情報
	VECTOR	_vHitDir;			// 被弾方向
	float	_fHitSpeed;			// 被弾速度
	float	_fHitSpeedDecay;	// 被弾速度減衰
	float	_fHitTime;			// 被弾時間

	// カメラ関連
	float	_cameraAngle;
	bool	_bIsDeathCameraSet;
};





//// まだ未使用のクラスたち
//class PlayerInput
//{
//public:
//
//	// 入力状態の取得
//	bool IsMoving() const;
//	bool IsDashing() const;
//	bool IsAttackPressed() const;
//	bool IsDodgePressed() const;
//
//	// 入力状態を設定する
//	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
//	{
//		_key = key;
//		_trg = trg;
//		_lx = lx;
//		_ly = ly;
//		_rx = rx;
//		_ry = ry;
//		_analogMin = analogMin;
//	}
//
//protected:
//	// 入力関係
//	int _key = 0;
//	int _trg = 0;
//	float _lx = 0.0f;
//	float _ly = 0.0f;
//	float _rx = 0.0f;
//	float _ry = 0.0f;
//	float _analogMin = 0.0f;
//};
//
//class PlayerMove
//{
//public:
//	void InitializePlayerConfig(PlayerConfig& config);	// プレイヤー設定初期化
//	void ProcessMovement(const PlayerInput& input, float cameraAngle);
//
//protected:
//	PlayerConfig _config;
//};
//
//class PlayerAnimator
//{
//
//};

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

