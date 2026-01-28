#pragma once
#include "appframe.h"

// 前方宣言
class CharaBase;
class PlayerBase;
class PlayerManager;
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
	bool GetDebugViewCollision() { return _bViewCollision; }
	void SetDebugViewCollision(bool bView) { _bViewCollision = bView; }
	bool GetUseCollision() { return _bUseCollision; }
	void SetUseCollision(bool bUse) { _bUseCollision = bUse; }

protected:
	// プレイヤー管理をPlayerManagerに委譲
	std::shared_ptr<PlayerManager> _playerManager;

	// スマートポインタで管理する
	// 同じオブジェクトを共有して、すべての参照がなくなったら解放される
	std::shared_ptr<StageBase>		_stage;			// ステージ
	std::shared_ptr<CameraManager>	_cameraManager;	// カメラマネージャー
	std::shared_ptr<GameCamera>		_gameCamera;	// ゲームカメラ
	std::shared_ptr<DebugCamera>	_debugCamera;	// デバッグカメラ
	std::shared_ptr<BulletManager>	_bulletManager;	// 弾マネージャー
	//std::shared_ptr<DodgeSystem>	_dodgeSystem;	// 回避システム
	std::shared_ptr<EnergyUI>		_energyUI;		// エネルギーUI

	std::shared_ptr<AbilitySelectScreen>_abilitySelectScreen;
	
	// シングルトン取得
	AttackManager* _attackManager = nullptr;
	EnergyManager* _energyManager = nullptr;

	// デバッグ用
	bool _bViewCollision;// 当たり判定表示
	bool _bUseCollision;// 当たり判定有効



	std::shared_ptr<LightManager> _lightManager;// ライトマネージャー 
	// 生成されたライトを管理
	std::vector<LightInfo> _lights;

private:
	void CheckCollisionCharaMap(std::shared_ptr<CharaBase> chara);										// キャラとマップの当たり判定
	void CheckHitPlayerEnemy(std::shared_ptr<CharaBase> chara1, std::shared_ptr<CharaBase> chara2);		// プレイヤーと敵の当たり判定
	void CheckHitCharaBullet(std::shared_ptr<CharaBase> chara);											// キャラと弾の当たり判定
	void CheckActiveAttack(std::shared_ptr<CharaBase> chara);											// 有効な攻撃のチェック
	void CheckHitCharaAttackCol(std::shared_ptr<CharaBase> chara, std::shared_ptr<AttackBase> attack);	// キャラと攻撃コリジョンの当たり判定
	void ConvertEnergy(std::shared_ptr<AttackBase> attack, float damage);								// ダメージをエネルギーに変換する



	// ライト関連
	void InitializeLights();// ライト初期化
	void ProcessLights();// ライト更新
	void TerminateLights();// ライトの終了処理

	// 指定座標にポイントライトを追加
	// ライトを生成して、コンテナに追加
	// ライトの位置、影響範囲、ライトの色
	int AddPointLight(VECTOR vPos, float fRange, COLOR_F color);

	void RemoveLight(int lightHandle);// 指定ライトを削除


	/// のうりょくせんたくがめんよう
	bool _isUseDebugScreen;

	// --- ここからアイテム描画用メンバ ---
	std::vector<VECTOR> _itemPositions;
	std::vector<unsigned int> _itemColors;
	float _itemRadius = 0.6f;
	float _itemHeight = 1.2f;
	VECTOR _playerStartPos = VGet(0.0f, 0.0f, 0.0f);

	// 開始位置周りにアイテムを生成
	void CreateItemsAtStart(int count = 3, float ringRadius = 2.0f);
};
