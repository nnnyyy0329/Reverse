// 担当 : 成田

#pragma once
#include "appframe.h"
#include "PlayerBase.h"
#include "AnimManager.h"
#include "AbilityBase.h"

// 前方宣言
class CameraManager;
class AbilitySelectScreen;

// プレイヤータイプ列挙型
enum class PLAYER_TYPE
{
	NONE,
	SURFACE,	// 表プレイヤー
	INTERIOR,	// 裏プレイヤー
	BULLET,		// 弾丸プレイヤー
	_EOT_,
};

class PlayerManager
{
public:
	PlayerManager();
	virtual ~PlayerManager();

	bool Initialize();
	bool Terminate();
	bool Process();
	bool Render();
	bool DebugRender();

	// プレイヤー管理
	bool RegisterPlayer(PLAYER_TYPE type, std::shared_ptr<PlayerBase> player);	// プレイヤーの登録関数
	void SwitchPlayerByAbility(ABILITY_TYPE ability);							// アビリティによるプレイヤー切り替え

	void SwitchPlayerByEnergy();												// エネルギーによるプレイヤー切り替え
	void SwitchPlayer(PLAYER_TYPE type);										// プレイヤー切り替え関数
	void EnableStateTransfer(bool enable) { _bEnableStateTransfer = enable; }	// 状態の引き継ぎ設定
	void TransferPlayerState(PlayerBase* oldPlayer, PlayerBase* newPlayer);		// 切り替え時の位置置き換え関数

	// 変身管理
	void StartTransform(PLAYER_TYPE targetType);				// 変身開始
	void SwitchPlayerImmediate(PLAYER_TYPE targetType);			// 即時変身
	void ProcessTransform();									// 変身処理
	void EndTransform();										// 変身終了
	bool IsTransforming() const { return _bIsTransforming; }	// 変身中か

	// 変身解除管理
	void StartTransformCancel();											// 変身解除開始
	void ProcessTransformCancel();											// 変身解除処理
	void EndTransformCancel();												// 変身解除終了
	bool IsTransformCanceling() const { return _bIsTransformCanceling; }	// 変身解除中か

	/*****ゲッターセッター*****/
	PlayerBase* GetActivePlayer() const;									// アクティブプレイヤー取得
	std::shared_ptr<PlayerBase> GetActivePlayerShared() const;				// アクティブプレイヤー取得(shared_ptr版)
	std::shared_ptr<PlayerBase> GetPlayerByType(PLAYER_TYPE type) const;	// タイプでプレイヤーを取得
	PLAYER_TYPE GetActivePlayerType() const { return _eActivePlayerType; }	// アクティブプレイヤーのタイプ取得
	//PlayerBase* GetActivePlayer()const { return _activePlayer; }			// アクティブプレイヤー取得

	// 入力状態を設定する
	void SetInput(int key, int trg, float lx, float ly, float rx, float ry, float analogMin)
	{
		_key = key;
		_trg = trg;
		_lx = lx;
		_ly = ly;
		_rx = rx;
		_ry = ry;
		_analogMin = analogMin;
	}

	// クラスセット
	void SetCameraManager(std::shared_ptr<CameraManager> cameraManager) { _cameraManager = cameraManager; }
	void SetAbilitySelectScreen(std::shared_ptr<AbilitySelectScreen> abilityScreen){_abilitySelectScreen = abilityScreen;}

protected:
	std::shared_ptr<CameraManager> _cameraManager;
	std::shared_ptr<AbilitySelectScreen> _abilitySelectScreen;
	std::unordered_map<PLAYER_TYPE, std::shared_ptr<PlayerBase>> _players;

	// アクティブプレイヤー
	PlayerBase* _activePlayer;
	PLAYER_TYPE _eActivePlayerType;

	// 変身関係
	PLAYER_TYPE _eTransformTarget;	// 変身先のプレイヤータイプ
	float _fTransformTime;			// 変身タイマー
	float _fTransformMaxTime;		// 変身にかかる時間
	bool _bIsTransforming;			// 変身中か

	// 変身解除関係
	float _fTransformCancelTime;	// 変身解除タイマー
	float _fTransformCancelMaxTime;	// 変身解除にかかる時間
	bool _bIsTransformCanceling;	// 変身解除中か

	// プレイヤー切り替え時の状態引き継ぎフラグ
	bool _bEnableStateTransfer;		

	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;

};