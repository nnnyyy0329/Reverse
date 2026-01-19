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

	VECTOR GetHomePos() const { return _vHomePos; }
	void SetHomePos(VECTOR pos) { _vHomePos = pos; }

	// AppFrameに移動予定
	// DrawLine3Dを組み合わせて3D空間に円を描画する関数
	// vCenter:中心座標, fRadius:半径, color:色, segment:分割数
	void DrawCircle3D(VECTOR vCenter, float fRadius, unsigned int color, int segments);

	// DrawLine3Dを組み合わせて3D空間に扇形を描画する関数
	// vCenter:中心座標, vDir:基準の向きベクトル, fRadius:半径, fHalfAngleDeg:半角(度), color:色, segments:分割数
	void DrawFan3D(VECTOR vCenter, VECTOR vDir, float fRadius, float fHalfAngleDeg, unsigned int color, int segments);

	void ChangeState(std::shared_ptr<EnemyState> newState);

	const EnemyParam& GetEnemyParam() const { return _enemyParam; }
	void SetEnemyParam(const EnemyParam& param);// パラメータを設定したときに視界のcos値を計算

	std::shared_ptr<CharaBase> GetTarget() const { return _targetPlayer; }
	void SetTarget(std::shared_ptr<CharaBase> target) { _targetPlayer = target; }

	// ターゲットチェック(索敵)
	bool IsTargetVisible(std::shared_ptr<CharaBase> target);// 視界内ならtrueを返す

	// 弾関連
	void SetBulletManager(std::shared_ptr<BulletManager> bulletManager) { _bulletManager = bulletManager; }// マネージャーをセット
	void SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime);// 発射リクエストをする

	// 攻撃コリジョン関連(ステート側で呼び出し)
	void StartAttack(const EnemyAttackSettings& settings);// 攻撃の開始
	void UpdateAttackTransform(const EnemyAttackSettings& settings);// 攻撃コリジョンの位置更新
	void StopAttack();// 攻撃の停止

	// 被ダメージ処理
	void ApplyDamage(float fDamage) override;

	// Enemyポインタを受け取ってEnemyStateのshared_ptrを返す関数
	// ↑をRecoveryHandlerとして定義
	// ハンドラ:何かが起きた時にどう対処するかの関数
	using RecoveryHandler = std::function<std::shared_ptr<EnemyState>(Enemy*)>;
	// ハンドラをセットする関数(Factoryで設定)
	void SetRecoveryHandler(RecoveryHandler handler) { _recoveryHandler = handler; }
	// ハンドラを実行する(ステート側から呼ばれる関数)
	std::shared_ptr<EnemyState> GetRecoveryState() const;

protected:

	VECTOR _vHomePos;// 敵の初期位置

	std::shared_ptr<CharaBase> _targetPlayer;// 追跡用

	std::shared_ptr<EnemyState> _currentState;
	EnemyParam _enemyParam;

	std::weak_ptr<BulletManager> _bulletManager;// マネージャーの参照を持つ(modegameが所有)

	std::shared_ptr<AttackBase> _attackCollision;// 攻撃コリジョン

	RecoveryHandler _recoveryHandler;// 関数を保存する変数

	
};

