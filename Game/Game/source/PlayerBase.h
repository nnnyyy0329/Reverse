// 担当 : 成田

#pragma once
#include "CharaBase.h"
#include "AttackBase.h"

// 攻撃定数構造体
struct AttackConstants
{
	float ATTACK_OFFSET_SCALE;		// 攻撃判定オフセット倍率
	int SURFACE_MAX_COMBO_COUNT;	// 表プレイヤー用コンボカウント
	int INTERIOR_MAX_COMBO_COUNT;	// 裏プレイヤー用コンボカウント
};

// 攻撃設定データ構造体
struct AttackConfig
{
	VECTOR topOffset;		// コリジョン上部位置
	VECTOR bottomOffset;	// コリジョン下部位置
	float radius;			// 半径
	float delay;			// 発生
	float duration;			// 持続
	float recovery;			// 硬直
	float damage;			// ダメージ
	std::string effectName;	// エフェクト名
	VECTOR effectOffset;	// エフェクト位置オフセット

};

// 状態列挙型
enum class PLAYER_STATUS
{
	NONE,
	WAIT,			// 待機
	WALK,			// 歩行
	RUN,			// 走行
	FIRST_ATTACK,	// 1段目攻撃
	SECOND_ATTACK,	// 2段目攻撃
	THIRD_ATTACK,	// 3段目攻撃
	FOURTH_ATTACK,	// 4段目攻撃
	FIFTH_ATTACK,	// 5段目攻撃
	JUMP_UP,		// ジャンプ（上昇）
	JUMP_DOWN,		// ジャンプ（下降）
	CROUCH_WAIT,	// しゃがみ待機
	CROUCH_WALK,	// しゃがみ歩行
	HIT,			// 被弾
	DODGE,			// 回避
	DEATH,			// 死亡
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

	virtual void ProcessPlayAnimation() = 0;	// アニメーション再生処理の仮想関数

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

	/*****ゲッターセッター*****/
	PLAYER_STATUS GetStatus() { return _ePlayerStatus; }	// 現在の状態を取得
	void SetStatus(PLAYER_STATUS e) { _ePlayerStatus = e; }	// 現在の状態を設定

	VECTOR GetAttackColTop(){ return _vAttackColTop; }			// 攻撃コリジョン上部
	VECTOR GetAttackColBottom(){ return _vAttackColBottom; }	// 攻撃コリジョン下部
	float GetAttackColR(){ return _fAttackColR; }				// 攻撃コリジョン半径

protected:	// 攻撃関係

	virtual AttackConstants GetAttackConstants() = 0;			// 攻撃定数を取得
	virtual void GetAttackConfigs(AttackConfig configs[3]) = 0;	// 攻撃設定を取得

	// PlayerBase_Attack.cppで定義
	void CallProcessAttack();		// 攻撃関係Process呼び出し用関数
	void ProcessAttackColPos();		// コリジョン位置の更新処理
	void ProcessAttack();			// 攻撃処理
	void ProcessBranchAttack();		// 攻撃分岐処理
	void InitializeAttackData();	// 攻撃データ初期化
	void ReceiveAttackColData();	// 攻撃コリジョンの情報受け取り関数
	bool CanNextAttack();			// 次の攻撃が可能かチェック
	bool IsAttacking();				// 攻撃中かチェック

	// 攻撃システム
	std::vector<std::shared_ptr<AttackBase>> _attacks;	// 攻撃配列
	std::vector<PLAYER_STATUS> _attackStatuses;			// 攻撃状態配列

private:	// 攻撃関係

	void UpdateAttackColPos(std::shared_ptr<AttackBase> attack, VECTOR& topOffset, VECTOR& bottomOffset, VECTOR& baseOffset);	// 攻撃判定の位置更新処理
	void ProcessStartAttack(int comboCount, PLAYER_STATUS nextStatus, std::shared_ptr<AttackBase> attack);						// 攻撃開始処理
	void ProcessAttackEffect(int attackIndex);																					// 攻撃エフェクト処理
	void ProcessComboAttack(int attackIndex);																					// コンボ攻撃処理
	void ProcessAttackFinish(std::shared_ptr<AttackBase> attack);																// 攻撃終了処理
	void EndAttackSequence();																									// 攻撃課程修了
	void ProcessNextAttack(int currentIndex);																					// 次の攻撃処理
	std::shared_ptr<AttackBase> GetAttackByStatus(PLAYER_STATUS status);														// 状態に対応する攻撃を取得
	int GetInstanceId();																										// ID取得関数
	int GetAttackIndexByStatus(PLAYER_STATUS status);																			// 状態から攻撃インデックスを取得

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

	// 攻撃コリジョン情報の受け取り用
	VECTOR _vAttackColTop;
	VECTOR _vAttackColBottom;
	float _fAttackColR;
	bool _bCanCombo;	// コンボ可能フラグ
	int _iComboCount;	// コンボカウント

	// カメラ角度
	float _cameraAngle;
};

