#pragma once
#include "GameObjectBase.h"
#include "AttackManager.h"
#include "EnergyManager.h"

enum class CHARA_TYPE
{
	NONE,

	PLAYER,
	ENEMY,
	BULLET,

	SURFACE_PLAYER,
	INTERIOR_PLAYER,
	BULLET_PLAYER,

	_EOT_,
};

// キャラクターベースクラス
// 自分自身のshared_ptrをほかで取得するためにenable_shared_from_thisを継承
class CharaBase : public GameObjectBase, public std::enable_shared_from_this<CharaBase>
{
public:
	CharaBase();
	virtual ~CharaBase();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ情報描画
	virtual void CollisionRender(); // コリジョン描画

	/*****ゲッターセッター*****/
	/* virtualにしているのは、将来的に継承先で死亡フラグの管理方法を変える可能性があるため */
	VECTOR GetCollisionTop()const{ return _vCollisionTop; }		// 当たり判定の上端
	void SetCollisionTop(VECTOR v) { _vCollisionTop = v; }		// 当たり判定の上端

	VECTOR GetCollisionBottom()const{ return _vCollisionBottom; }	// 当たり判定の下端
	void SetCollisionBottom(VECTOR v) { _vCollisionBottom = v; }	// 当たり判定の下端

	float GetCollisionR()const{ return _fCollisionR; }	// 当たり判定の半径
	void SetCollisionR(float f) { _fCollisionR = f; }	// 当たり判定の半径

	float GetCollisionHeight()const{ return _fCollisionHeight; }	// 当たり判定の高さ
	void SetCollisionHeight(float f) { _fCollisionHeight = f; }		// 当たり判定の高さ

	float GetColSubY()const{ return _colSubY; }	// コリジョン判定時のY補正(腰位置）

	float GetMoveSpeed()const{ return _fMoveSpeed; }	// 移動速度
	void SetMoveSpeed(float f) { _fMoveSpeed = f; }		// 移動速度

	float GetDirSpeed()const{ return _fDirSpeed; }	// 回転速度
	void SetDirSpeed(float f) { _fDirSpeed = f; }	// 回転速度

	float GetLife()const{ return _fLife; }	// 体力
	void SetLife(float f) { _fLife = f; }	// 体力

	float GetMaxLife()const{ return _fMaxLife; }	// 最大体力
	void SetMaxLife(float f) { _fMaxLife = f; }		// 最大体力

	float GetGravity()const{ return _fGravity; }	// 重力
	void SetGravity(float f) { _fGravity = f; }		// 重力

	virtual bool GetIsDead()const{ return _bIsDead; }	// 死亡フラグ
	void SetIsDead(bool b) { _bIsDead = b; }			// 死亡フラグ

	CHARA_TYPE GetCharaType()const{ return _eCharaType; }			// キャラタイプ
	void SetCharaType(CHARA_TYPE eType) { _eCharaType = eType; }	// キャラタイプ

	// 被ダメージ処理
	virtual void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo);	// 攻撃による被ダメージ処理
	virtual void ApplyDamageByBullet(float fDamage, CHARA_TYPE eType);										// 弾による被ダメージ処理

protected:
	// キャラカプセルの当たり判定用
	VECTOR _vCollisionTop;		// 当たり判定の上端
	VECTOR _vCollisionBottom;	// 当たり判定の下端
	float _fCollisionR;			// 当たり判定の半径
	float _fCollisionHeight;	// 当たり判定の高さ

	// 3Dモデル描画用
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	// デバッグ用
	bool	_bViewCollision;

	// 基礎ステータス
	float _fMoveSpeed;	// 移動速度
	float _fDirSpeed;	// 回転速度
	float _fLife;		// 体力
	float _fMaxLife;	// 最大体力
	float _fGravity;	// 重力	
	bool _bIsDead;		// 死亡フラグ

	// キャラタイプ
	CHARA_TYPE _eCharaType;
};

