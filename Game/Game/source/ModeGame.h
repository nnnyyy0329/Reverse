#pragma once
#include "appframe.h"

// 前方宣言
class CharaBase;
class PlayerBase;
class PlayerManager;
class AttackBase;
class StageBase;
class CameraManager;
class DebugCamera;
class BulletManager;
class AttackManager;
class EnergyManager;

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

	// デバッグ関連
	bool GetDebugViewCollision() { return _bViewCollision; }
	void SetDebugViewCollision(bool bView) { _bViewCollision = bView; }
	bool GetUseCollision() { return _bUseCollision; }
	void SetUseCollision(bool bUse) { _bUseCollision = bUse; }

private:
	void CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara);										// キャラとマップの当たり判定
	void CheckHitPlayerEnemy(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2);		// プレイヤーと敵の当たり判定
	void CheckHitCharaBullet(std::shared_ptr<CharaBase> chara);											// キャラと弾の当たり判定
	void CheckActiveAttack(std::shared_ptr<CharaBase> chara);											// 有効な攻撃のチェック
	void CheckHitCharaAttackCol(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack);	// キャラと攻撃コリジョンの当たり判定
	void ConvertEnergy(std::shared_ptr<AttackBase> attack, float damage);								// ダメージをエネルギーに変換する

protected:
	// プレイヤー管理をPlayerManagerに委譲
	std::shared_ptr<PlayerManager> _playerManager;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase> _stage;// ステージ
	std::shared_ptr<CameraManager> _cameraManager;// カメラマネージャー
	std::shared_ptr<DebugCamera> _debugCamera;// デバッグカメラ
	std::shared_ptr<BulletManager> _bulletManager;// 弾マネージャー
	
	// シングルトン取得
	AttackManager* _attackManager = nullptr;
	EnergyManager* _energyManager = nullptr;

	// デバッグ用
	bool _bViewCollision;// 当たり判定表示
	bool _bUseCollision;// 当たり判定有効
};
