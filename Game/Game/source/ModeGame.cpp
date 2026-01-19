#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "ModeMenu.h"

#include "CharaBase.h"
#include "StageBase.h"
#include "Enemy.h"
#include "DebugCamera.h"

#include "CameraManager.h"
#include "BulletManager.h"
#include "AttackManager.h"

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"

bool ModeGame::Initialize() 
{
	if (!base::Initialize()) { return false; }

	// PlayerManagerの初期化
	_playerManager = std::make_shared<PlayerManager>();
	_playerManager->Initialize();

	// BulletManagerの初期化
	_bulletManager = std::make_shared<BulletManager>();
	_bulletManager->Initialize();

	// プレイヤーの作成と登録
	{
		auto surfacePlayer = std::make_shared<SurfacePlayer>();
		surfacePlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::SURFACE, surfacePlayer);

		auto interiorPlayer = std::make_shared<InteriorPlayer>();
		interiorPlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::INTERIOR, interiorPlayer);
	}

	// いったんこれ
	_stage = std::make_shared<StageBase>(2);
	_cameraManager = std::make_shared<CameraManager>();
	_cameraManager->SetTarget(_playerManager->GetPlayerByType(PLAYER_TYPE::SURFACE));

	_debugCamera = std::make_shared<DebugCamera>();

	// 敵設定
	for (const auto& enemy : _stage->GetEnemies()) {
		enemy->SetTarget(_playerManager->GetActivePlayerShared());
		enemy->SetBulletManager(_bulletManager);
	}

	return true;
}

bool ModeGame::Terminate() 
{
	base::Terminate();

	// プレイヤー開放
	_playerManager.reset();

	return true;
}

bool ModeGame::Process()
{
	base::Process();

	/// 入力取得
	{
		int key = ApplicationMain::GetInstance()->GetKey();
		int trg = ApplicationMain::GetInstance()->GetTrg();
		auto analog = ApplicationMain::GetInstance()->GetAnalog();
		float lx = analog.lx;
		float ly = analog.ly;
		float rx = analog.rx;
		float ry = analog.ry;
		float analogMin = ApplicationMain::GetInstance()->GetAnalogMin();

		// プレイヤーマネージャーに入力状態を渡す
		if (_playerManager)
		{
			_playerManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if (_cameraManager)
		{
			_cameraManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
	}

	// spaceキーでメニューを開く
	if (ApplicationMain::GetInstance()->GetTrg() & PAD_INPUT_10) {
		ModeMenu* modeMenu = new ModeMenu();
		modeMenu->SetDebugCamera(_debugCamera);// デバッグカメラを渡す
		_debugCamera->SetInfo(_cameraManager->GetVPos(), _cameraManager->GetVTarget());// 元カメラの情報を渡す
		// ModeGameより上のレイヤーにメニューを登録する
		ModeServer::GetInstance()->Add(modeMenu, 99, "menu");
	}

	// オブジェクトの更新
	{
		_playerManager->Process();
		_stage->Process();
		_bulletManager->Process();
		AttackManager::GetInstance().Process();
	}

	// 当たり判定
	{
		auto player = _playerManager->GetActivePlayerShared();
		auto enemies = _stage->GetEnemies();

		// マップ
		//CheckCollisionCharaMap(player);
		//for (const auto& enemy : enemies) {
		//	CheckCollisionCharaMap(enemy);
		//}


		// 弾
		CheckHitCharaBullet(player);
		for (const auto& enemy : enemies) {
			CheckHitCharaBullet(enemy);
		}

		// プレイヤーと敵の当たり判定
		for (const auto& enemy : enemies)
		{
			CheckHitPlayerEnemy(player, enemy);
		}

		// キャラと攻撃コリジョンの当たり判定
		CheckHitCharaAttack(player);									// プレイヤー
		for(const auto& enemy : enemies){ CheckHitCharaAttack(enemy); }	// 敵
	}

	// いったん
	std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
	_cameraManager->SetTarget(activePlayer);	// 毎フレームプレイヤーにカメラを合わせる
	// 敵にターゲットのプレイヤーを設定
	for (const auto& enemy : _stage->GetEnemies()) {
		enemy->SetTarget(activePlayer);
	}

	_cameraManager->Process();

	return true;
}

bool ModeGame::Render() 
{
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// ライト設定
	{
		SetUseLighting(TRUE);

		#if 1	// 平行ライト
			SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
			ChangeLightTypeDir(VGet(-1, -1, 0));
		#endif
		#if 0	// ポイントライト
			PlayerBase* activePlayer = _playerManager->GetActivePlayer();

			SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
			ChangeLightTypePoint(VAdd(activePlayer->GetPos(), VGet(0, 50.f, 0)), 1000.f, 0.f, 0.005f, 0.f);
		#endif
	}

	// カメラ設定
	{
		// メニューが開いていて、デバッグカメラが使われているなら
		auto* menu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
		if (menu && menu->IsUseDebugCamera() && _debugCamera) {
			_debugCamera->SetUp();// デバッグカメラ設定更新
		}
		else {
			_cameraManager->SetUp();// カメラ設定更新
		}
	}
	
	// オブジェクトの描画
	{
		_playerManager->Render();
		_stage->Render();
		_bulletManager->Render();
		AttackManager::GetInstance().Render();
	}

	// デバッグ情報の描画
	{
		_playerManager->DebugRender();
		_stage->DebugRender();
		_debugCamera->DebugRender();
		AttackManager::GetInstance().DrawDebug();
	}

	return true;
}

// キャラと弾の当たり判定
void ModeGame::CheckHitCharaBullet(std::shared_ptr<CharaBase> chara){
	if (!chara) return;

	CHARA_TYPE myType = chara->GetCharaType();// 自分のキャラタイプを取得

	const auto& bullets = _bulletManager->GetBullets();// 弾のリストを取得

	std::vector<std::shared_ptr<Bullet>> deadBullets;// 削除する弾を一時保存するリスト

	// 全弾ループ
	for (const auto& bullet : bullets){
		if (!bullet) continue;

		// キャラと弾のタイプが同じなら無視する
		if (bullet->GetShooterType() == myType){
			continue;
		}

		// 当たり判定
		if(HitCheck_Capsule_Sphere(
			chara->GetCollisionTop(), chara->GetCollisionBottom(), chara->GetCollisionR(),
			bullet->GetPos(), bullet->GetCollisionR()
		)) {
			// 当たった

			// ダメージ処理とか

			deadBullets.push_back(bullet);// 削除リストに追加
		}
	}

	// 全ての判定が終わった後に、まとめて削除する
	for (const auto& deadBullet : deadBullets) {
		_bulletManager->RemoveBullet(deadBullet);
	}
}
