#pragma once
#include "appframe.h"

// 前方宣言
class CharaBase;
class PlayerBase;
class PlayerManager;
class AttackBase;
class StageBase;
class CameraManager;
class GameCamera;
class DebugCamera;
class BulletManager;
class AttackManager;
class EnergyManager;
class DodgeSystem;

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();


	// 消す
	bool GetDebugViewColloion() { return _bViewCollision; }
	void SetDebugViewColloion(bool b) { _bViewCollision = b; }
	bool GetDebugUseColloion() { return _bUseCollision; }
	void SetDebugUseColloion(bool b) { _bUseCollision = b; }
	bool GetDebugViewCameraInfo() { return _bViewCameraInfo; }
	void SetDebugViewCameraInfo(bool b) { _bViewCameraInfo = b; }


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
	std::shared_ptr<StageBase>		_stage;			// ステージ
	std::shared_ptr<CameraManager>	_cameraManager;	// カメラマネージャー
	std::shared_ptr<GameCamera>		_gameCamera;	// ゲームカメラ
	std::shared_ptr<DebugCamera>	_debugCamera;	// デバッグカメラ
	std::shared_ptr<BulletManager>	_bulletManager;	// 弾マネージャー
	//std::shared_ptr<DodgeSystem>	_dodgeSystem;	// 回避システム
	
	// シングルトン取得
	AttackManager* _attackManager = nullptr;
	EnergyManager* _energyManager = nullptr;



	// 消す
	bool	_bViewCollision;
	bool	_bUseCollision;
	bool	_bViewCameraInfo;
};
