#pragma once
#include "appframe.h"

//いったんこれ
class CharaBase;
class PlayerBase;
class PlayerManager;
class AttackBase;
class StageBase;
class CameraManager;
class DebugCamera;

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

private:
	void CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara);										// キャラとマップの当たり判定
	void CheckHitPlayerEnemy(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2);		// プレイヤーと敵の当たり判定
	void CheckHitCharaBullet(std::shared_ptr<CharaBase> chara, std::shared_ptr<CharaBase> bullet);		// キャラと弾の当たり判定
	void CheckHitCharaAttackCol(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack);	// キャラと攻撃コリジョンの当たり判定

protected:

	// プレイヤー管理をPlayerManagerに委譲
	//std::shared_ptr<PlayerManager> _playerManager;
	at::vsp<PlayerManager> _playerManager;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase> _stage;// ステージ
	std::shared_ptr<CameraManager> _cameraManager;// カメラマネージャー
	std::shared_ptr<DebugCamera> _debugCamera;// デバッグカメラ
};
