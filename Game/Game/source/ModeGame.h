#pragma once
#include "appframe.h"
#include "CharaBase.h"
#include "AttackManager.h"

// 前方宣言
class CharaBase;
class PlayerBase;
class PlayerManager;
class PlayerLifeBarUI;
class AttackBase;
class StageBase;
class CameraManager;
class GameCamera;
class DebugCamera;
class BulletManager;
class AttackManager;
class EnergyManager;
class EnergyUI;
class DodgeSystem;
class LightManager;
class Item;

class AbilitySelectScreen;

// ライト情報構造体
struct LightInfo
{
	int handle;// ライトハンドル
	VECTOR vPos;// ライト位置
	bool bisActive;// ライト有効フラグ
};

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
	bool GetDebugViewDebugInfo() { return _bViewDebugInfo; }
	void SetDebugViewDebugInfo(bool bView) { _bViewDebugInfo = bView; }
	bool GetDebugViewCollision() { return _bViewCollision; }
	void SetDebugViewCollision(bool bView) { _bViewCollision = bView; }
	bool GetDebugUseCollision() { return _bUseCollision; }
	void SetDebugUseCollision(bool bUse) { _bUseCollision = bUse; }
	// カメラマネージャー取得
	std::shared_ptr<CameraManager> GetCameraManager() const { return _cameraManager; }
	// ゲームカメラ取得
	std::shared_ptr<GameCamera> GetGameCamera() const { return _gameCamera; }
	// デバッグカメラ取得
	std::shared_ptr<DebugCamera> GetDebugCamera() const { return _debugCamera; }

protected:
	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase>			_stage;					// ステージ
	std::shared_ptr<GameCamera>			_gameCamera;			// ゲームカメラ
	std::shared_ptr<DebugCamera>		_debugCamera;			// デバッグカメラ
	//std::shared_ptr<DodgeSystem>		_dodgeSystem;			// 回避システム
	std::shared_ptr<AbilitySelectScreen>_abilitySelectScreen;	// 能力選択画面

	// 管理関連
	std::shared_ptr<PlayerManager>		_playerManager;			// プレイヤーマネージャー
	std::shared_ptr<CameraManager>		_cameraManager;			// カメラマネージャー
	std::shared_ptr<BulletManager>		_bulletManager;			// 弾マネージャー
	std::shared_ptr<LightManager>		_lightManager;			// ライトマネージャー 

	// UI関連
	std::shared_ptr<EnergyUI>			_energyUI;				// エネルギーUI
	std::shared_ptr<PlayerLifeBarUI>	_playerLifeBarUI;		// プレイヤーライフバーUI

	// シングルトン取得
	AttackManager* _attackManager = nullptr;
	EnergyManager* _energyManager = nullptr;

	// デバッグ用
	bool _bViewDebugInfo;	// デバッグ情報表示
	bool _bViewCollision;	// 当たり判定表示
	bool _bUseCollision;	// 当たり判定有効

	// ベクターコンテナ
	std::vector<LightInfo>	_lights;	// 生成されたライトを管理

	// のうりょくせんたくがめんよう
	bool _isUseDebugScreen;

	std::shared_ptr<Item>               _item;                  //　アイテム

private:
	void CheckCollisionCharaMap	(std::shared_ptr<CharaBase> chara);										// キャラとマップの当たり判定
	void CheckHitPlayerEnemy	(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2);	// プレイヤーと敵の当たり判定
	void CheckHitCharaBullet	(std::shared_ptr<CharaBase> chara);										// キャラと弾の当たり判定
	void CheckActiveAttack		(std::shared_ptr<CharaBase> chara);										// 有効な攻撃のチェック
	void CheckHitCharaAttackCol	(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack);	// キャラと攻撃コリジョンの当たり判定
	void CheckHitCharaItem(std::shared_ptr<CharaBase> chara, std::shared_ptr <Item>item);               // アイテムとプレイヤーの当たり判定
	void ConvertEnergy			(std::shared_ptr<AttackBase> attack, float damage);						// ダメージをエネルギーに変換する
	bool OwnerIsAttackingOwner	(CHARA_TYPE charaType, ATTACK_OWNER_TYPE ownerType);					// 攻撃所有者が自分に攻撃しているかどうか



	// ライト関連
	void InitializeLights();// ライト初期化
	void ProcessLights();// ライト更新
	void TerminateLights();// ライトの終了処理

	// 指定座標にポイントライトを追加
	// ライトを生成して、コンテナに追加
	// ライトの位置、影響範囲、ライトの色
	int AddPointLight(VECTOR vPos, float fRange, COLOR_F color);

	void RemoveLight(int lightHandle);// 指定ライトを削除

};
