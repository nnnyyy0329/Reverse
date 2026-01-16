#pragma once
#include "CharaBase.h"
#include "EnemyState.h"

class BulletManager;

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
	// center:中心座標, radius:半径, color:色, segment:分割数
	void DrawCircle3D(VECTOR center, float radius, unsigned int color, int segments);

	void ChangeState(std::shared_ptr<EnemyState> newState);

	const EnemyParam& GetEnemyParam() const { return _enemyParam; }
	void SetEnemyParam(const EnemyParam& param);// パラメータを設定したときに視界のcos値を計算

	std::shared_ptr<CharaBase> GetTarget() const { return _targetPlayer; }
	void SetTarget(std::shared_ptr<CharaBase> target) { _targetPlayer = target; }

	// 弾関連
	void SetBulletManager(std::shared_ptr<BulletManager> bulletManager) { _bulletManager = bulletManager; }// マネージャーをセット
	void SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime);// 発射リクエストをする

	std::vector<std::shared_ptr<Bullet>> GetBullet(){ return _bullets; }// 指定したインデックスの弾を取得

protected:

	VECTOR _vHomePos;// 敵の初期位置

	std::shared_ptr<CharaBase> _targetPlayer;// 追跡用

	std::shared_ptr<EnemyState> _currentState;
	EnemyParam _enemyParam;

	std::weak_ptr<BulletManager> _bulletManager;// マネージャーの参照を持つ(modegameが所有)
};

