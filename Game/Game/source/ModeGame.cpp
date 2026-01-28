#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "ModeMenu.h"

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
#include "EnergyUI.h"
#include "DodgeSystem.h"

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"

#include "AbilitySelectScreen.h"

// メニュー項目
class MenuDebugCamera : public MenuItemBase
{
public:
	MenuDebugCamera(void* param, std::string text) : MenuItemBase(param, text) {}

	void SetCameraManagerMenu(std::shared_ptr<CameraManager> cameraManager) { __cameraManager = cameraManager; }
	void SetGameCameraMenu(std::shared_ptr<GameCamera> gameCamera) { __gameCamera = gameCamera; }
	void SetDebugCameraMenu(std::shared_ptr<DebugCamera> debugCamera) { __debugCamera = debugCamera; }

	// 項目を決定したらこの関数が呼ばれる
	// return int : 0 = メニュー継続, 1 = メニュー終了
	virtual int Selected() 
	{
		int key = ApplicationMain::GetInstance()->GetKey();
		int trg = ApplicationMain::GetInstance()->GetTrg();
		auto analog = ApplicationMain::GetInstance()->GetAnalog();
		float lx = analog.lx;
		float ly = analog.ly;
		float rx = analog.rx;
		float ry = analog.ry;
		float analogMin = ApplicationMain::GetInstance()->GetAnalogMin();

		// デバッグカメラ
		{
			__debugCamera->SetInfo(__gameCamera->GetVPos(), __gameCamera->GetVTarget());// 元カメラの情報を渡す
			__cameraManager->SetIsUseDebugCamera(true);

			auto* modeMenu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
			modeMenu->SetUseDebugCamera(true);
		}

		return 1;
	}

protected:

	std::shared_ptr<CameraManager>	__cameraManager;	// カメラマネージャー
	std::shared_ptr<DebugCamera>	__debugCamera;	// デバッグカメラ
	std::shared_ptr<GameCamera>	__gameCamera;		// ゲームカメラ

};


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
	_abilitySelectScreen = std::make_shared<AbilitySelectScreen>();
	_abilitySelectScreen->Initialize();
	_isUseDebugScreen = false;

	// エネルギーUI初期化
	_energyUI = std::make_shared<EnergyUI>();
	_energyUI->Initialize();

	// ライトの初期化
	InitializeLights();

	// アイテム初期化
	_playerStartPos = VGet(0.0f, 0.0f, 0.0f); // 必要ならステージやプレイヤーから取得する
	CreateItemsAtStart(3, 2.0f);

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



	// 能力選択画面のデバッグ寄関数
	if(trg & PAD_INPUT_8)
	{
		_isUseDebugScreen = !_isUseDebugScreen;
	}
	if(_isUseDebugScreen)
	{
		_abilitySelectScreen->Process();
	}



	// spaceキーでメニューを開く
	if (ApplicationMain::GetInstance()->GetTrg() & PAD_INPUT_10)
	{
		ModeMenu* modeMenu = new ModeMenu();
		modeMenu->SetCameraManager(_cameraManager);

		// デバッグカメラ
		auto debugCamera = new MenuDebugCamera(this, "DebugCamera");
		debugCamera->SetCameraManagerMenu(_cameraManager);
		debugCamera->SetDebugCameraMenu(_debugCamera);
		debugCamera->SetGameCameraMenu(_gameCamera);


		ModeServer::GetInstance()->Add(modeMenu, 99, "menu");
		modeMenu->AddMenuItem(debugCamera);
	}

	// クラスセット
	{
		_cameraManager->SetGameCamera(_gameCamera);
		_cameraManager->SetDebugCamera(_debugCamera);
	}

	// オブジェクトの更新
	{
		_playerManager->Process();
		_stage->Process();
		_bulletManager->Process();
		AttackManager::GetInstance()->Process();
		//_dodgeSystem->Process();
		_energyUI->Process();
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

	auto player = _playerManager->GetActivePlayerShared();
	if(player)
	{
		// 描画と同じ計算を使う（ModeGame::Render と合わせる）
		const float itemHeight = 100.0f;
		const float itemRadius = 30.0f;
		const float itemZOffset = 500.0f;

		for(size_t i = 0; i < _itemPositions.size(); )
		{
			// アイテムのカプセル上下中心
			VECTOR base = _itemPositions[i];
			VECTOR itemTop = VGet(base.x, base.y + itemHeight, base.z + itemZOffset);   // 上端中心
			VECTOR itemBottom = VGet(base.x, base.y, base.z + itemZOffset);             // 下端中心

			// 既存の当たり判定関数を使用（カプセル対カプセル）
			if(HitCheck_Capsule_Capsule(
				player->GetCollisionTop(), player->GetCollisionBottom(), player->GetCollisionR(),
				itemTop, itemBottom, itemRadius
			))
			{
				// ヒットしたらアイテムを削除（色配列も同期して削除）
				_itemPositions.erase(_itemPositions.begin() + i);
				if(i < _itemColors.size()) { _itemColors.erase(_itemColors.begin() + i); }

				// 必要ならエフェクトや音を再生する処理をここに追加
				// ループインデックスは増やさず次の要素を処理
				continue;
			}

			++i;
		}
	}


	// ターゲット更新
	{
		std::shared_ptr<PlayerBase> activePlayer = _playerManager->GetActivePlayerShared();
		_gameCamera->SetTarget(activePlayer);	// 毎フレームプレイヤーにカメラを合わせる

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
		// メニューが開いていて、デバッグカメラが使われているなら
		//auto* menu = dynamic_cast<ModeMenu*>(ModeServer::GetInstance()->Get("menu"));
		//if(menu && menu->IsUseDebugCamera() && _debugCamera)
		//{
		//	_debugCamera->SetUp();// デバッグカメラ設定更新
		//}
		//else
		//{
		//	_gameCamera->SetUp();// カメラ設定更新
		//}
	}
	
	// オブジェクトの描画
	{
		_stage->Render();
		_playerManager->Render();
		_bulletManager->Render();
		AttackManager::GetInstance()->Render();
		_energyUI->Render();

		// アイテム描画
		for(size_t i = 0; i < _itemPositions.size(); ++i)
		{
			// カプセルの上下端を作る（Y軸方向に伸ばす）
			// base をアイテム位置に使い、Z方向にオフセットを加える
			const float itemHeight = 100.0f;      // カプセルの高さ
			const float itemRadius = 30.0f;       // カプセルの半径
			const float itemZOffset = 500.0f;     // ← ここを大きくすると Z 軸方向により移動します

			VECTOR base = _itemPositions[i];
			VECTOR Pos1 = VGet(base.x, base.y, base.z + itemZOffset); // 下端の中心
			VECTOR Pos2 = VGet(base.x, base.y + itemHeight, base.z + itemZOffset); // 上端の中心

			unsigned int col = (i < _itemColors.size()) ? _itemColors[i] : GetColor(255, 255, 255);
			DrawCapsule3D(
				Pos1,                   // Pos1：カプセルの端1の中心座標
				Pos2,                   // Pos2：カプセルの端2の中心座標
				itemRadius,             // r：半径
				16,                     // DivNum：細かさ
				GetColor(255, 0, 0),    // DifColor：メインの色（赤）
				GetColor(255, 255, 255),// SpcColor：光が反射した時の色（白）
				TRUE                    // FillFlag：塗りつぶすかどうか（TRUEで塗りつぶし）
			);
		}

		// のうりょk選択画面
		if(_isUseDebugScreen)
		{
			_abilitySelectScreen->Render();
		}
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
		//_debugCamera->DebugRender();
		_cameraManager->Render();
		//AttackManager::GetInstance()->DebugRender();
		EnergyManager::GetInstance()->DebugRender();
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

void ModeGame::CreateItemsAtStart(int count, float ringRadius)
{
	_itemPositions.clear();
	_itemColors.clear();

	// プレイヤー開始位置は _playerStartPos にある想定
	// （もしプレイヤーやステージから開始位置が取れるならここで代入してください）
	// 例: if (_stage) _playerStartPos = _stage->GetPlayerStartPos();

	for (int i = 0; i < count; ++i) {
		float ang = (2.0f * 3.14159265358979323846f) * (float(i) / float(count));
		float x = _playerStartPos.x + cosf(ang) * ringRadius;
		float z = _playerStartPos.z + sinf(ang) * ringRadius;
		float y = _playerStartPos.y; // 地面と同じ高さに置く場合
		_itemPositions.push_back(VGet(x, y, z));

		// 色
		int r = 160 + (rand() % 96);
		int g = 160 + (rand() % 96);
		int b = 160 + (rand() % 96);
		_itemColors.push_back(GetColor(r, g, b));
	}
}
