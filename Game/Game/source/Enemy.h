#pragma once
#include "CharaBase.h"
#include "EnemyState.h"
#include "EnemyAttackSettings.h"

class BulletManager;
class AttackBase;

class Enemy : public CharaBase
{
public:
	Enemy();
	virtual ~Enemy();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ描画
	virtual void CollisionRender(); // コリジョン描画
	void DrawLifeBar();// 体力バー描画

	VECTOR GetHomePos() const { return _vHomePos; }
	void SetHomePos(VECTOR pos) { _vHomePos = pos; }

	bool CanRemove() const { return _bCanRemove; }// delete可能か

	const EnemyParam& GetEnemyParam() const { return _enemyParam; }
	void SetEnemyParam(const EnemyParam& param);// パラメータを設定したときに視界のcos値を計算

	std::shared_ptr<CharaBase> GetTarget() const { return _targetPlayer; }
	void SetTarget(std::shared_ptr<CharaBase> target) { _targetPlayer = target; }

	int GetDamageCount() const { return _damageCnt; }// 何回ダメージを受けたか

	// モデル名のゲッターセッター
	const std::string& GetModelName() const { return _modelName; }
	void SetModelName(const std::string name) { _modelName = name; }

	// ステート変更
	void ChangeState(std::shared_ptr<EnemyState> newState);

	// 弾関連
	void SetBulletManager(std::shared_ptr<BulletManager> bulletManager) { _bulletManager = bulletManager; }// マネージャーをセット
	void SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime);// 発射リクエストをする

	// 攻撃コリジョン関連(ステート側で呼び出し)
	void StartAttack(const EnemyAttackSettings& settings);// 攻撃の開始
	void UpdateAttackTransform(const EnemyAttackSettings& settings);// 攻撃コリジョンの位置更新
	void StopAttack();// 攻撃の停止

	// 被ダメージ処理
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType) override;
	void ResetDamageCount() { _damageCnt = 0; }// ダメージカウントリセット

	// 死亡判定
	bool IsDead() const;
	void EnableRemove() { _bCanRemove = true; }// delete可能にする

	// Enemyポインタを受け取ってEnemyStateのshared_ptrを返す関数
	// ↑をRecoveryHandlerとして定義
	// ハンドラ:何かが起きた時にどう対処するかの関数
	using RecoveryHandler = std::function<std::shared_ptr<EnemyState>(Enemy*)>;
	// ハンドラをセットする関数(Factoryで設定)
	void SetRecoveryHandler(RecoveryHandler handler) { _recoveryHandler = handler; }
	// ハンドラを実行する(ステート側から呼ばれる関数)
	std::shared_ptr<EnemyState> GetRecoveryState() const;

	// 索敵関連
	bool ShouldUpdateSearch() { return _searchTimer == 0; }// 索敵更新タイミングかどうか
	void UpdateSearchTimer();// 索敵タイマー更新
	bool IsTargetDetected() { return _bIsTargetDetected; };// 索敵結果を取得
	void SetTargetDetected(bool bDetected) { _bIsTargetDetected = bDetected; };// 索敵結果を設定



	// AppFrameに移動予定
	// DrawLine3Dを組み合わせて3D空間に円を描画する関数
	// vCenter:中心座標, fRadius:半径, color:色, segment:分割数
	void DrawCircle3D(VECTOR vCenter, float fRadius, unsigned int color, int segments);

	// DrawLine3Dを組み合わせて3D空間に扇形を描画する関数
	// vCenter:中心座標, vDir:基準の向きベクトル, fRadius:半径, fHalfAngleDeg:半角(度), color:色, segments:分割数
	void DrawFan3D(VECTOR vCenter, VECTOR vDir, float fRadius, float fHalfAngleDeg, unsigned int color, int segments);

	// 滑らかな向き変更
	// 目標角度に向かって滑らかに回転する
	// 目標角度(ラジアン)、旋回速度(度/フレーム)
	bool SmoothTurnToAngle(float fTargetAngle, float fTurnSpeedDeg);

	// 目標方向ベクトルに向かって滑らかに回転する
	// 目標方向ベクトル、旋回速度(度/フレーム)
	bool SmoothTurnToDirection(VECTOR vTargetDir, float fTurnSpeedDeg);

	// 目標位置に向かって滑らかに回転する
	// 目標位置、旋回速度(度/フレーム)
	bool SmoothTurnToPosition(VECTOR vTargetPos, float fTurnSpeedDeg);

protected:

	VECTOR _vHomePos;// 敵の初期位置

	std::shared_ptr<CharaBase> _targetPlayer;// 追跡用

	std::shared_ptr<EnemyState> _currentState;
	EnemyParam _enemyParam;

	std::weak_ptr<BulletManager> _bulletManager;// マネージャーの参照を持つ(modegameが所有)

	std::shared_ptr<AttackBase> _attackCollision;// 攻撃コリジョン

	RecoveryHandler _recoveryHandler;// 関数を保存する変数

	int _lifeBarHandle = -1;// 体力バー用
	int _lifeBarFrameHandle = -1;// 体力バー枠用

	int _damageCnt = 0;// 何回ダメージを受けたか(スタン用)

	bool _bIsExist = true;// 生存フラグ
	bool _bCanRemove = false;// delete可能フラグ

	std::string _modelName = "Enemy";// モデル名

	// 索敵関連
	int _searchTimer = 0;// 索敵タイマー
	bool _bIsTargetDetected = false;// ターゲットが見つかったか

private:
	void LoadEnemyModel();// モデルを名前に応じて読み込む

	static float NormalizeAngleDiff(float fAngleDiff);// 角度差を-PI~+PIの範囲に正規化する
};

