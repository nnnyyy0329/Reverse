#pragma once
#include "appframe.h"

//いったんこれ
class CharaBase;
class PlayerBase;
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

protected:
	void CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara);// キャラとマップの当たり判定

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<PlayerBase> _player;// プレイヤー
	std::shared_ptr<StageBase> _stage;// ステージ
	std::shared_ptr<CameraManager> _cameraManager;// カメラマネージャー
	std::shared_ptr<DebugCamera> _debugCamera;// デバッグカメラ
};
