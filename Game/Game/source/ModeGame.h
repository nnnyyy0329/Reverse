#pragma once
#include "appframe.h"

//いったんこれ
class PlayerBase;
class PlayerManager;
class StageBase;
class CameraManager;

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

	//いったんこれ
	//std::shared_ptr<PlayerBase> _surfacePlayer;
	//std::shared_ptr<PlayerBase> _interiorPlayer;

	// プレイヤー管理をPlayerManagerに委譲
	std::shared_ptr<PlayerManager> _playerManager;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	//std::shared_ptr<PlayerBase> _player;// プレイヤー
	std::shared_ptr<StageBase> _stage;// ステージ
	std::shared_ptr<CameraManager> _cameraManager;// カメラマネージャー
};
