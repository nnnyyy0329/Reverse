#pragma once

#include "CharaBase.h"
#include "EnemyState.h"
#include "EnemyAttackSettings.h"

class BulletManager;
class AttackBase;
class StageBase;

class Enemy : public CharaBase
{
public:
	Enemy();
	virtual ~Enemy();

	virtual bool Initialize();	// 初期化
	virtual bool Terminate();	// 終了
	virtual bool Process();		// 更新
	virtual bool Render();		// 描画
	virtual void DebugRender();	// デバッグ描画
	virtual void CollisionRender(); // コリジョン描画
	void DrawLifeBar();// ライフバー描画

	VECTOR GetHomePos() { return _vHomePos; }
	void SetHomePos(VECTOR pos) { _vHomePos = pos; }

	bool CanRemove() { return _bCanRemove; }// delete可能か

	const EnemyParam& GetEnemyParam() const { return _enemyParam; }
	void SetEnemyParam(const EnemyParam& param);// パラメータを設定したときに視界のcos値を計算

	std::shared_ptr<CharaBase> GetTarget() { return _targetPlayer; }
	void SetTarget(std::shared_ptr<CharaBase> target) { _targetPlayer = target; }

	// モデル名のゲッターセッター
	const std::string& GetModelName() const { return _modelName; }
	void SetModelName(const std::string name) { _modelName = name; }

	// ステート変更
	void ChangeState(std::shared_ptr<EnemyState> newState);

	// 弾関連
	void SetBulletManager(std::shared_ptr<BulletManager> bulletManager) { _bulletManager = bulletManager; }// マネージャーをセット
	void SpawnBullet(VECTOR vStartPos, VECTOR vDir, float fRadius, float fSpeed, int lifeTime);// 発射リクエストをする

	// 攻撃コリジョン関連(ステート側で呼び出し)
	void StartAttack(const EnemyAttackSettings& settings);// 攻撃の開始
	void UpdateAttackTransform(const EnemyAttackSettings& settings);// 攻撃コリジョンの位置更新
	void StopAttack();// 攻撃の停止

	// 被ダメージ処理
	void ApplyDamage(float fDamage, ATTACK_OWNER_TYPE eType, const ATTACK_COLLISION& attackInfo) override;

	// 死亡判定
	bool IsDead();
	void EnableRemove() { _bCanRemove = true; }// delete可能にする


	// 被ダメ後の遷移先を決定する
	using AfterDamageStateSelector = std::function<std::shared_ptr<EnemyState>(Enemy*, int)>;
	void SetAfterDamageStateSelector(AfterDamageStateSelector selector) { _afterDamageStateSelector = selector; }
	std::shared_ptr<EnemyState> GetAfterDamageStateSelector(int comboCnt);
	// ダウン後の遷移先を決定する
	using AfterDownStateSelector = std::function<std::shared_ptr<EnemyState>(Enemy*)>;
	void SetAfterDownStateSelector(AfterDownStateSelector selector) { _afterDownStateSelector = selector; }
	std::shared_ptr<EnemyState> GetAfterDownStateSelector();

	// 索敵関連
	bool ShouldUpdateSearch() { return _searchTimer == 0; }// 索敵更新タイミングかどうか
	void UpdateSearchTimer();// 索敵タイマー更新
	bool IsTargetDetected() { return _bIsTargetDetected; };// 索敵結果を取得
	void SetTargetDetected(bool bDetected) { _bIsTargetDetected = bDetected; };// 索敵結果を設定

	// 索敵の障害物チェック
	void SetStage(std::shared_ptr<StageBase> stage) { _stage = stage; }// ステージ参照をセット
	bool CheckLineOfSight(VECTOR vStart, VECTOR vEnd);// 視線が通っているか(障害物チェック)

	// 移動可能範囲チェック
	bool CheckInsideMoveArea(VECTOR vPos);// 指定座標が移動可能範囲内かどうか
	bool IsOutSideMoveArea() { return _bIsOutSideMoveArea; }
	bool CorrectPosToMoveArea();// 初期位置方向へ押し戻す

	// 連続被ダメカウント管理
	int GetDamageComboCnt() { return _damageComboCnt; }
	void UpdateDamageCombo();// 連続被ダメカウントを更新
	void UpdateDamageComboTimer();// リセットタイマー更新

	// 徐々に回転させる
	void SmoothRotateTo(VECTOR vTargetDir, float turnSpeedDeg);// 目標方向へ指定速度で回転

protected:

	VECTOR _vHomePos;// 敵の初期位置

	std::shared_ptr<CharaBase> _targetPlayer;// 接近用

	std::shared_ptr<EnemyState> _currentState;
	EnemyParam _enemyParam;

	std::weak_ptr<BulletManager> _bulletManager;// マネージャーの参照を持つ(modegameが所有)

	std::shared_ptr<AttackBase> _attackCollision;// 攻撃コリジョン

	AfterDamageStateSelector _afterDamageStateSelector;
	AfterDownStateSelector _afterDownStateSelector;

	int _lifeBarHandle = -1;// ライフバー用
	int _lifeBarFrameHandle = -1;// ライフバー枠用

	bool _bIsExist = true;// 生存フラグ
	bool _bCanRemove = false;// delete可能フラグ

	std::string _modelName = "Enemy";// モデル名

	// 索敵関連
	int _searchTimer = 0;// 索敵タイマー
	bool _bIsTargetDetected = false;// ターゲットが見つかったか

	std::weak_ptr<StageBase> _stage;// ステージ参照

	COLOR_F _defaultColor;// 元のマテリアルカラーを保存
	bool _bIsColorChanged = false;// カラー変更中かどうか

	int _damageComboCnt = 0;// 連続被ダメ回数
	float _fDamageComboResetTimer = 0.0f;// リセットタイマー

	bool _bIsOutSideMoveArea = false;// エリア外へ移動しようとしたか

private:
	void LoadEnemyModel();// モデルを名前に応じて読み込む

	void UpdateColorChange();// カラー変更中

};

