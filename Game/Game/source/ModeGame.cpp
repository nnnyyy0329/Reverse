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
#include "EnergyManager.h"

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"

#include "LightManager.h"

bool ModeGame::Initialize() 
{
	if (!base::Initialize()) { return false; }

	// Manager初期化
	{
		// PlayerManagerの初期化
		_playerManager = std::make_shared<PlayerManager>();
		_playerManager->Initialize();

		// BulletManagerの初期化
		_bulletManager = std::make_shared<BulletManager>();
		_bulletManager->Initialize();

		// LightManagerの初期化
		_lightManager = std::make_shared<LightManager>();
		_lightManager->Initialize();
	}

	// シングルトンインスタンスを取得
	_attackManager = AttackManager::GetInstance();
	_energyManager = EnergyManager::GetInstance();

	// プレイヤーの作成と登録
	{
		auto surfacePlayer = std::make_shared<SurfacePlayer>();
		surfacePlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::SURFACE, surfacePlayer);

		auto interiorPlayer = std::make_shared<InteriorPlayer>();
		interiorPlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::INTERIOR, interiorPlayer);
	}

	// ステージ初期化
	_stage = std::make_shared<StageBase>(3);// ステージ番号で切り替え

	// カメラ初期化
	{
		_cameraManager = std::make_shared<CameraManager>();
		_cameraManager->SetTarget(_playerManager->GetPlayerByType(PLAYER_TYPE::SURFACE));
		_debugCamera = std::make_shared<DebugCamera>();
	}

	// 敵設定
	for (const auto& enemy : _stage->GetEnemies()) {
		enemy->SetTarget(_playerManager->GetActivePlayerShared());
		enemy->SetBulletManager(_bulletManager);
	}

	// ライトの初期化
	InitializeLights();

	return true;
}

bool ModeGame::Terminate() 
{
	base::Terminate();

	// ライトの終了処理
	TerminateLights();

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
		AttackManager::GetInstance()->Process();
	}

	// ライト更新
	ProcessLights();

	// 当たり判定
	{
		auto player = _playerManager->GetActivePlayerShared();
		auto enemies = _stage->GetEnemies();

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
		CheckActiveAttack(player);										// プレイヤー
		for(const auto& enemy : enemies){ CheckActiveAttack(enemy); }	// 敵

		// マップ
		CheckCollisionCharaMap(player);
		for (const auto& enemy : enemies) {
			CheckCollisionCharaMap(enemy);
		}
	}

	// ターゲット更新
	{
		std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
		_cameraManager->SetTarget(activePlayer);	// 毎フレームプレイヤーにカメラを合わせる
		// 敵にターゲットのプレイヤーを設定
		for (const auto& enemy : _stage->GetEnemies()) {
			enemy->SetTarget(activePlayer);
		}
	}

	// エフェクト更新
	EffectServer::GetInstance()->Update();

	// カメラ更新
	_cameraManager->Process();

	return true;
}

bool ModeGame::Render() 
{
	base::Render();

	// 3D基本設定
	{
		SetUseZBuffer3D(TRUE);
		SetWriteZBuffer3D(TRUE);
		SetUseBackCulling(TRUE);
	}

	// ライト設定
	{
		// ライティングを有効化
		_lightManager->SetLightEnable(true);

		// 標準ライトをディレクショナルライトとして設定
		_lightManager->SetLightType(LightManager::LIGHT_TYPE::DIRECTIONAL);
		_lightManager->SetDirectionalLightDir(VGet(-1.0f, -1.0f, -1.0f));

		// グローバルアンビエントライト設定
		_lightManager->SetAmbientLight(GetColorF(0.3f, 0.3f, 0.3f, 0.0f));
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
		_stage->Render();
		_playerManager->Render();
		_bulletManager->Render();
		AttackManager::GetInstance()->Render();
	}

	// エフェクト描画
	EffectServer::GetInstance()->Render();

	// デバッグ情報の描画
	{
		_stage->DebugRender();
		_debugCamera->DebugRender();
		//AttackManager::GetInstance()->DebugRender();
		//EnergyManager::GetInstance()->DebugRender();

		// ライト情報
		DrawFormatString(10, 100, GetColor(255, 255, 255), "有効なライト : %d", _lights.size());
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

void ModeGame::InitializeLights()
{
	// ライトコンテナをクリア
	_lights.clear();

	// ライトを追加
	// test
	AddPointLight(VGet(0.0f, 500.0f, 0.0f), 1000.0f, GetColorF(1.0f, 1.0f, 1.0f, 0.0f));
}

void ModeGame::ProcessLights()
{
	// 各ライトの更新処理
	// キャラの位置に追従するライトなど
}

void ModeGame::TerminateLights()
{
	// すべてのライトを削除
	for (auto& lightInfo : _lights)
	{
		if (lightInfo.handle != -1)
		{
			DeleteLightHandle(lightInfo.handle);
			lightInfo.handle = -1;
		}
	}

	// ライトコンテナをクリア
	_lights.clear();
}

int ModeGame::AddPointLight(VECTOR vPos, float fRange, COLOR_F color)
{
	// ポイントライトを生成
	int lightHandle = _lightManager->CreatePointLight(vPos, fRange, color);

	if (lightHandle == -1) { return -1; }// 生成失敗

	// ライト情報を作成
	LightInfo lightInfo;
	lightInfo.handle = lightHandle;
	lightInfo.vPos = vPos;
	lightInfo.bisActive = true;

	// コンテナに追加
	_lights.push_back(lightInfo);

	return lightHandle;
}

void ModeGame::RemoveLight(int lightHandle)
{
	// コンテナから該当するライトを検索
	for (auto it = _lights.begin(); it != _lights.end(); ++it)
	{
		if (it->handle == lightHandle)
		{
			// ライトハンドルを削除
			DeleteLightHandle(lightHandle);

			// コンテナから削除
			_lights.erase(it);
			break;
		}
	}
}
