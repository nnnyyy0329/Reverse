#pragma once
#include "appframe.h"

//いったんこれ
class CharaBase;
class PlayerBase;
class PlayerManager;
class StageBase;
class CameraManager;
class DebugCamera;
class BulletManager;

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

protected:
	void CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara);// キャラとマップの当たり判定
	void CheckHitCharaBullet(std::shared_ptr<CharaBase> chara);// キャラと弾の当たり判定

	// プレイヤー管理をPlayerManagerに委譲
	std::shared_ptr<PlayerManager> _playerManager;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase> _stage;// ステージ
	std::shared_ptr<CameraManager> _cameraManager;// カメラマネージャー
	std::shared_ptr<DebugCamera> _debugCamera;// デバッグカメラ
	std::shared_ptr<BulletManager> _bulletManager;// 弾マネージャー
};
