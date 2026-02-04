#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "ModeMenu.h"
#include "ModeStageChange.h"

#include "CharaBase.h"
#include "StageBase.h"
#include "Enemy.h"

#include "CameraManager.h"
#include "GameCamera.h"
#include "DebugCamera.h"

#include "BulletManager.h"
#include "AttackManager.h"
#include "EnergyManager.h"
#include "LightManager.h"

#include "DodgeSystem.h"
#include "AbilitySelectScreen.h"

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"
#include "BulletPlayer.h"

#include "EnergyUI.h"
#include "PlayerLifeBarUI.h"

#include "MenuItemBase.h"

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
	{
		_attackManager = AttackManager::GetInstance();
		_energyManager = EnergyManager::GetInstance();
	}

	// プレイヤーの作成と登録
	{
		auto surfacePlayer = std::make_shared<SurfacePlayer>();
		surfacePlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::SURFACE, surfacePlayer);

		auto interiorPlayer = std::make_shared<InteriorPlayer>();
		interiorPlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::INTERIOR, interiorPlayer);

		auto bulletPlayer = std::make_shared<BulletPlayer>();
		bulletPlayer->Initialize();
		_playerManager->RegisterPlayer(PLAYER_TYPE::BULLET, bulletPlayer);
		bulletPlayer->SetBulletManager(_bulletManager);
	}

	// ステージ初期化
	_currentStageNum = 1;
	_stage = std::make_shared<StageBase>(_currentStageNum);// ステージ番号で切り替え

	// カメラ初期化
	{
		_cameraManager = std::make_shared<CameraManager>();

		_gameCamera = std::make_shared<GameCamera>();
		_gameCamera->SetTarget(_playerManager->GetPlayerByType(PLAYER_TYPE::SURFACE));

		_debugCamera = std::make_shared<DebugCamera>();
	}

	// 回避システム初期化
	{
		//_dodgeSystem = std::make_shared<DodgeSystem>();
		//_dodgeSystem->Initialize();
	}

	// 敵設定
	for (const auto& enemy : _stage->GetEnemies())
	{
		enemy->SetTarget(_playerManager->GetActivePlayerShared());
		enemy->SetBulletManager(_bulletManager);
	}

	// 能力選択画面初期化
	{
		_abilitySelectScreen = std::make_shared<AbilitySelectScreen>();
		_abilitySelectScreen->Initialize();
	}

	// UI初期化
	{
		// エネルギーUI初期化
		_energyUI = std::make_shared<EnergyUI>();
		_energyUI->Initialize();

		// ライフバーUI初期化
		_playerLifeBarUI = std::make_shared<PlayerLifeBarUI>();
		_playerLifeBarUI->Initialize();
	}

	// ライトの初期化
	InitializeLights();

	// デバッグ関連初期化
	{
		_bViewDebugInfo = false;
		_bViewCollision = false;
		_bUseCollision = true;
	}

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
	
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	auto analog = ApplicationMain::GetInstance()->GetAnalog();
	float lx = analog.lx;
	float ly = analog.ly;
	float rx = analog.rx;
	float ry = analog.ry;
	float analogMin = ApplicationMain::GetInstance()->GetAnalogMin();

	/// 入力取得
	{
		// プレイヤーマネージャーに入力状態を渡す
		if(_playerManager)
		{
			_playerManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// カメラマネージャーに入力状態を渡す
		if(_cameraManager)
		{
			_cameraManager->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
		// 能力選択画面に入力状態を渡す
		if(_abilitySelectScreen)
		{
			_abilitySelectScreen->SetInput(key, trg, lx, ly, rx, ry, analogMin);
		}
	}



	// 能力選択画面のデバッグ関数
	if(_abilitySelectScreen && _abilitySelectScreen->GetIsSelectComplete())
	{
		ABILITY_TYPE selectedAbility = _abilitySelectScreen->GetSelectedAbility();
		_playerManager->SwitchPlayerByAbility(selectedAbility);
		_abilitySelectScreen->ResetSelection(); // 選択状態をリセット
	}



	// spaceキーでメニューを開く
	if (ApplicationMain::GetInstance()->GetTrg() & PAD_INPUT_10)
	{
		ModeMenu* modeMenu = new ModeMenu();
		ModeServer::GetInstance()->Add(modeMenu, 99, "menu");

		modeMenu->SetCameraManager(_cameraManager);

		// メニュー項目を作成
		auto viewDebugInfo = new MenuItemViewDebugInfo(this, "ViewDebugInfo");
		auto viewCollision = new MenuItemViewCollision(this, "ViewCollision");
		auto useCollision = new MenuItemUseCollision(this, "UseCollision");
		auto debugCamera = new MenuDebugCamera(this, "DebugCamera");

		// カメラ情報を設定
		debugCamera->SetCameraManagerMenu(_cameraManager);
		debugCamera->SetDebugCameraMenu(_debugCamera);
		debugCamera->SetGameCameraMenu(_gameCamera);

		modeMenu->AddMenuItem(viewDebugInfo);
		modeMenu->AddMenuItem(viewCollision);
		modeMenu->AddMenuItem(useCollision);
		modeMenu->AddMenuItem(debugCamera);
	}

	// クラスセット
	{
		_cameraManager->SetGameCamera(_gameCamera);
		_cameraManager->SetDebugCamera(_debugCamera);
		_playerLifeBarUI->SetPlayerManager(_playerManager);
	}

	// オブジェクトの更新
	{
		_playerManager->Process();
		_stage->Process();
		_bulletManager->Process();
		AttackManager::GetInstance()->Process();
		//_dodgeSystem->Process();
		_energyUI->Process();
		_playerLifeBarUI->Process();
		_abilitySelectScreen->Process();
	}

	// ライト更新
	ProcessLights();

	// 当たり判定
	{
		auto player = _playerManager->GetActivePlayerShared();
		auto& enemies = _stage->GetEnemies();

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
		if (_bUseCollision)
		{
			CheckCollisionCharaMap(player);
			for (const auto& enemy : enemies) {
				CheckCollisionCharaMap(enemy);
			}
		}

		// トリガー
		CheckHitPlayerTrigger(player);
	}

	// ターゲット更新
	{
		std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();

		_gameCamera->SetTarget(activePlayer);							// 毎フレームプレイヤーにカメラを合わせる
		activePlayer->SetCameraAngle(_gameCamera->GetCameraAngleH());	// プレイヤーにカメラ角度を設定

		// 敵にターゲットのプレイヤーを設定
		for (const auto& enemy : _stage->GetEnemies()) 
		{
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
		_cameraManager->SwitchCameraSetUp();
	}
	
	// オブジェクトの描画
	{
		_stage->Render();
		_playerManager->Render();
		_bulletManager->Render();
		_energyUI->Render();
		_playerLifeBarUI->Render();
		_abilitySelectScreen->Render();
	}

	// コリジョンの描画
	if (_bViewCollision)
	{
		_stage->CollisionRender();
		AttackManager::GetInstance()->CollisionRender();
	}

	// エフェクト描画
	EffectServer::GetInstance()->Render();

	// デバッグ情報の描画
	if (_bViewDebugInfo)
	{
		// 0,0,0を中心に線を引く
		{
			float linelength = 1000.f;
			VECTOR v = { 0, 0, 0 };
			DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
			DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
			DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
		}

		_stage->DebugRender();
		AttackManager::GetInstance()->DebugRender();
		_debugCamera->DebugRender();
		_gameCamera->DebugRender();

		// ライト情報
		DrawFormatString(10, 100, GetColor(255, 255, 255), "有効なライト : %d", _lights.size());

		//AttackManager::GetInstance()->DebugRender();
		EnergyManager::GetInstance()->DebugRender();

		// プレイヤーデバッグ情報
		std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
		activePlayer->DebugRender();

		_cameraManager->SwitchCameraDebugRender();
	}
	
	// コリジョンの描画
	if (_bViewCollision)
	{
		_stage->CollisionRender();
		AttackManager::GetInstance()->CollisionRender();

		// プレイヤーコリジョン描画
		std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
		activePlayer->CollisionRender();
	}

	_energyUI->Render();
	_cameraManager->SwitchCameraRender();

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

void ModeGame::RequestStageChange(int nextStageNum)
{
	if (_bIsStageChanging) { return; }// すでに切り替え中

	_bIsStageChanging = true;

	// ModeStageChangeを追加
	auto modeStageChange = new ModeStageChange(this, nextStageNum);
	ModeServer::GetInstance()->Add(modeStageChange, 50, "stageChange");
}

void ModeGame::ChangeStage(std::shared_ptr<StageBase> newStage, int stageNum)
{
	if (!newStage) { return; }

	// 現在のステージ番号を更新
	_currentStageNum = stageNum;

	// 古いステージを削除
	_stage.reset();

	// 新しいステージを設定
	_stage = newStage;

	// プレイヤーの位置をリセット
	VECTOR vStartPos = VGet(0.0f, 0.0f, 0.0f);

	auto activePlayer = _playerManager->GetActivePlayerShared();
	activePlayer->SetPos(vStartPos);

	// 敵の再設定
	for (const auto& enemy : _stage->GetEnemies())
	{
		enemy->SetTarget(_playerManager->GetActivePlayerShared());
		enemy->SetBulletManager(_bulletManager);
	}

	// オブジェクトの削除
	{
		_bulletManager->ClearAllBullets();
		AttackManager::GetInstance()->ClearAllAttacks();
	}

	// 切り替え完了
	_bIsStageChanging = false;
}