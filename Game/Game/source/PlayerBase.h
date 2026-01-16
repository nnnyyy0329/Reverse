// 担当 : 成田

#pragma once
#include "CharaBase.h"
#include "AttackBase.h"

// 攻撃定数構造体
struct AttackConstants
{
	float ATTACK_OFFSET_SCALE;
	float COMMON_RADIUS;
	float COMMON_DELAY;
	float COMMON_DURATION;
	float COMMON_RECOVERY;
	float NORMAL_DAMAGE;
	float FINISHER_DAMAGE;
	int MAX_COMBO_COUNT;
};

// 攻撃設定データ構造体
struct AttackConfig
{
	VECTOR topOffset;
	VECTOR bottomOffset;
	float damage;
};

// 状態列挙型
enum class PLAYER_STATUS
{
	NONE,
	WAIT,
	WALK,
	FIRST_ATTACK,
	SECOND_ATTACK,
	THIRD_ATTACK,
	JUMP_UP,
	JUMP_DOWN,
	CROUCH_WAIT,
	CROUCH_WALK,
	DEATH,
	_EOT_,
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

	/*****ゲッターセッター*****/
	// キャラの状態 
	PLAYER_STATUS GetStatus() { return _ePlayerStatus; }	// 現在の状態を取得
	void SetStatus(PLAYER_STATUS e) { _ePlayerStatus = e; }	// 現在の状態を設定

	// 攻撃コリジョン情報の受け取り用
	VECTOR GetAttackColTop(){ return _vAttackColTop; }
	VECTOR GetAttackColBottom(){ return _vAttackColBottom; }
	float GetAttackColR(){ return _fAttackColR; }

protected:	// 攻撃関係

	virtual AttackConstants GetAttackConstants() = 0;			// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[3]) = 0;	// 攻撃設定を取得

	// PlayerBase_Attack.cppで定義
	void CallProcessAttack();		// 攻撃関係Process呼び出し用関数																				// 攻撃関係Process呼び出し用関数
	void ProcessAttackColPos();		// コリジョン位置の更新処理
	void ProcessAttack();			// 攻撃処理
	void ProcessBranchAttack();		// 攻撃分岐処理
	void ProcessFirstAttack();		// 第1攻撃処理
	void ProcessSecondAttack();		// 第2攻撃処理
	void ProcessThirdAttack();		// 第3攻撃処理
	void InitializeAttackData();	// 攻撃データ初期化
	void ReceiveAttackColData();	// 攻撃コリジョンの情報受け取り関数
	bool CanNextAttack();			// 次の攻撃が可能かチェック
	bool IsAttacking();				// 攻撃中かチェック

	// 攻撃システム
	std::shared_ptr<AttackBase> _firstAttack;	// 第1攻撃
	std::shared_ptr<AttackBase> _secondAttack;	// 第2攻撃
	std::shared_ptr<AttackBase> _thirdAttack;	// 第3攻撃

private:	// 攻撃関係

	void UpdateAttackColPos(std::shared_ptr<AttackBase> attack, VECTOR& topOffset, VECTOR& bottomOffset, VECTOR& baseOffset);									// 攻撃判定の位置更新処理
	void ProcessStartAttack(int comboCount, PLAYER_STATUS nextStatus, std::shared_ptr<AttackBase> attack);														// 攻撃開始処理
	void ProcessComboAttack(std::shared_ptr<AttackBase> currentAttack, int nextComboCount, PLAYER_STATUS nextStatus, std::shared_ptr<AttackBase> nextAttack);	// 汎用コンボ攻撃処理
	void ProcessAttackFinish(std::shared_ptr<AttackBase> attack);																								// 攻撃終了処理
	void EndAttackSequence();																																	// 攻撃課程修了
	std::shared_ptr<AttackBase> GetAttackByStatus(PLAYER_STATUS status);																						// 状態に対応する攻撃を取得
	int GetInstanceId();																																		// ID取得関数

protected:

	PLAYER_STATUS _ePlayerStatus;		// キャラの状態
	PLAYER_STATUS _eOldPlayerStatus;	// 前フレームのキャラの状態

	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;


	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// デバッグ用
	bool	_bViewCollision;


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

	// 攻撃コンボ関係
	bool _bCanCombo;	// コンボ可能フラグ
	int _iComboCount;	// コンボカウント

	// 攻撃コリジョン情報の受け取り用
	VECTOR _vAttackColTop;
	VECTOR _vAttackColBottom;
	float _fAttackColR;
};

