#pragma once
#include "appframe.h"

class Enemy;
class CharaBase;

// ステート優先度
enum class STATE_PRIORITY
{
	NORMAL = 0,// 通常
	DAMAGE = 50,// 被ダメージ
	HIGH = 75,// 高優先度(中断されないアクションなど)
	TOP = 100,// 最優先(Dead, Stun, Down)
};

// 敵パラメータ構造体
struct EnemyParam
{
	float fMoveSpeed = 2.0f;// 敵の基本移動速度

	float fVisionRange = 0.0f;// 敵の索敵距離
	float fVisionAngle = 0.0f;// 敵の視界の角度

	float fAttackRange = 0.0f;// これ以内なら攻撃する距離
	float fChaseLimitRange = 0.0f;// これ以上離れたら接近をやめる距離

	float fTurnSpeed = 60.0f;// 旋回速度(度 / フレーム)

	float fIdleTime = 0.0f;// 待機時間
	float fMoveTime = 0.0f;// 徘徊時間
	float fDetectTime = 0.0f;// 発見硬直
	float fAttackTime = 0.0f;// 攻撃時間

	float fMaxLife = 100.0f;// 最大体力

	// 共通ステートのアニメーション名
	const char* animDamage = "Damage";
	const char* animDead = "Dead";
	const char* animStun = "Stun";
	const char* animDown = "Down";
};

// ターゲット情報構造体
struct TargetInfo
{
	std::shared_ptr<CharaBase> target;// ターゲットへのポインタ
	VECTOR vToTarget;// ターゲットへのベクトル
	VECTOR vDir;// ターゲットへの方向ベクトル(正規化)
	float fDist;// ターゲットまでの距離
	bool bExist;// ターゲットが存在するか
};

// 敵ステート基底クラス
class EnemyState
{
public:
	virtual ~EnemyState() = default;

	virtual void Enter(Enemy* owner) {}// その状態の切り替わった直後に一度だけ呼ばれる
	virtual std::shared_ptr<EnemyState> Update(Enemy* owner) = 0;// 毎フレーム呼ばれる。状態遷移する場合は新しい状態を返す。継続する場合はnullptrを返す
	virtual void Exit(Enemy* owner) {}// 別の状態に切り替わる直前に一度だけ呼ばれる。後始末はここで

	virtual const char* GetName() { return "None"; }// デバッグ用に状態名を取得

	virtual bool IsChasing() { return false; }// 接近状態かどうか

	virtual bool CanChangeState() { return true; }// ステート変更可能かどうか

	virtual STATE_PRIORITY GetPriority() { return STATE_PRIORITY::NORMAL; }// ステートの優先度を取得(デフォルトはNORMAL)

	virtual void UpdateSearch(Enemy* owner) {};// 索敵処理(各ステートでオーバーライド)

protected:
	// baseTime に -range, +range のランダム幅で増減した値を返す
	float CalcRandomRangeTime(float fBaseTime, float fRange);

	// ターゲット情報を取得(ターゲットへのベクトル、距離を計算)
	TargetInfo GetTargetInfo(Enemy* owner);

	// ターゲットが存在しない場合の処理(LostTargetへ)
	template<typename LostTargetState>
	std::shared_ptr<EnemyState> TransitionToLostNoTarget(Enemy* owner);

	// 追跡限界距離チェック(これ以上離れたらLostTargetへ)
	template<typename LostTargetState>
	std::shared_ptr<EnemyState> TransitionToLostOverChaseLimit(Enemy* owner, float fDist);

	// 移動可能範囲外チェック(範囲外ならIdleへ) : 徘徊ステート
	template<typename IdleState>
	std::shared_ptr<EnemyState> TransitionToIdleOutsideArea(Enemy* owner);

	// 移動可能範囲外チェック(範囲外ならLostTargetへ) : 追跡系ステート
	template<typename LostTargetState>
	std::shared_ptr<EnemyState> TransitionToLostOutsideArea(Enemy* owner);

	// ターゲット方向へ回転
	void RotateToTarget(Enemy* onwer, const VECTOR& vDir, float fRotSpeed);

	// ターゲット方向へ移動
	void MoveToTarget(Enemy* onwer, const VECTOR& vDir, float fMoveSpeed);

	// 移動停止
	void StopMove(Enemy* owner);

	float _fTimer = 0.0f;
	float _fTargetTimer = 0.0f;
	float _fAnimSpeed = 1.0f;

};
