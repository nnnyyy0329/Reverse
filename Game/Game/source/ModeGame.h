#pragma once
#include "appframe.h"
#include "CharaBase.h"

// 前方宣言
class PlayerBase;
class PlayerManager;

class AttackBase;
//class ShieldBase;
class StageBase;
class DodgeSystem;

class CameraManager;
class GameCamera;
class DebugCamera;
class AimCamera;

class BulletManager;
class AttackManager;
class EnergyManager;
class StaminaManager;
class LightManager;

class PlayerLifeBarUI;
class EnergyUI;
class StaminaUI;

//class Item;

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

	// ステージ切り替え関連
	void RequestStageChange(int nextStageNum);// ステージ切り替えリクエスト
	void ChangeStage(std::shared_ptr<StageBase> newStage, int stageNum);// ステージ切り替え実行
	void RestartCurrentStage();// 現在のステージをリスタート
	void SetPlayerConfig(VECTOR vPos, VECTOR vRot);// jsonからの設定を適用(プレイヤー)
	int GetCurrentStageNum() { return _currentStageNum; }

protected:
	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase>			_stage;					// ステージ
	std::shared_ptr<GameCamera>			_gameCamera;			// ゲームカメラ
	std::shared_ptr<DebugCamera>		_debugCamera;			// デバッグカメラ
	std::shared_ptr<AimCamera>			_aimCamera;				// エイムカメラ
	///std::shared_ptr<ShieldBase>			_shieldBase;			// シールドベース
	std::shared_ptr<DodgeSystem>		_dodgeSystem;			// 回避システム
	std::shared_ptr<AbilitySelectScreen>_abilitySelectScreen;	// 能力選択画面
	//std::shared_ptr<Item>               _item;                  //　アイテム

	// 管理関連
	std::shared_ptr<PlayerManager>		_playerManager;			// プレイヤーマネージャー
	std::shared_ptr<CameraManager>		_cameraManager;			// カメラマネージャー
	std::shared_ptr<BulletManager>		_bulletManager;			// 弾マネージャー
	std::shared_ptr<LightManager>		_lightManager;			// ライトマネージャー 

	// UI関連
	std::shared_ptr<EnergyUI>			_energyUI;				// エネルギーUI
	std::shared_ptr<StaminaUI>			_staminaUI;				// スタミナUI
	std::shared_ptr<PlayerLifeBarUI>	_playerLifeBarUI;		// プレイヤーライフバーUI

	// ベクターコンテナ
	std::vector<LightInfo>	_lights;	// 生成されたライトを管理

	// シングルトン取得
	AttackManager* _attackManager = nullptr;
	EnergyManager* _energyManager = nullptr;

	// デバッグ用
	bool _bViewDebugInfo;	// デバッグ情報表示
	bool _bViewCollision;	// 当たり判定表示
	bool _bUseCollision;	// 当たり判定有効

	bool _bIsStageChanging;// ステージ切り替え中フラグ
	int _currentStageNum;// 現在のステージ番号

private:
	void CheckCollisionCharaMap	(std::shared_ptr<CharaBase> chara);// キャラとマップの当たり判定
	void CheckCollisionCharaChara(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2);// キャラ同士の当たり判定
	void CheckCollisionCameraMap();// カメラとマップの当たり判定
	void CheckHitCharaBullet	(std::shared_ptr<CharaBase> chara);// キャラと弾の当たり判定
	void CheckHitPlayerTrigger(std::shared_ptr<CharaBase> player);// プレイヤーとトリガーの当たり判定



	void CheckActiveAttack		(std::shared_ptr<CharaBase> chara);													// 有効な攻撃のチェック
	void CheckHitCharaAttackCol	(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack);				// キャラと攻撃コリジョンの当たり判定
	void ConvertEnergy			(std::shared_ptr<AttackBase> attack, float damage);									// ダメージをエネルギーに変換する
	void CheckHitAbsorbAttack	(std::shared_ptr<CharaBase> chara);													// 吸収攻撃の当たり判定チェック関数
	void CheckHitCharaAbsorbCol	(std::shared_ptr<CharaBase> chara, const PlayerAbsorbAttackSystem* absorbSystem);	// キャラと吸収攻撃の当たり判定
	bool OwnerIsAttackingOwner	(CHARA_TYPE charaType, ATTACK_OWNER_TYPE ownerType);								// 攻撃所有者が自分に攻撃しているかどうか
	bool OwnerIsAbsorbingOwner	(std::shared_ptr<CharaBase>chara);													// 攻撃所有者が自分に攻撃しているかどうか(吸収攻撃用)

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
