// 担当 : 成田

#pragma once
#include "appframe.h"
#include "PlayerBase.h"
#include "AbilitySelectScreen.h"

// 前方宣言
class CameraManager;

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

	// プレイヤー管理
	bool RegisterPlayer(PLAYER_TYPE type, std::shared_ptr<PlayerBase> player);	// プレイヤーの登録関数
	void SwitchPlayerByInput();													// 入力によるプレイヤー切り替え
	void SwitchPlayerByAbility(ABILITY_TYPE ability);							// アビリティによるプレイヤー切り替え
	void SwitchPlayerByEenrgy();												// エネルギーによるプレイヤー切り替え
	void SwitchPlayer(PLAYER_TYPE type);										// プレイヤー切り替え関数
	void EnableStateTransfer(bool enable) { _bEnableStateTransfer = enable; }	// 状態の引き継ぎ設定
	void TransferPlayerState(PlayerBase* oldPlayer, PlayerBase* newPlayer);		// 切り替え時の位置置き換え関数

	/*****ゲッターセッター*****/
	PlayerBase* GetActivePlayer() const;									// アクティブプレイヤー取得
	std::shared_ptr<PlayerBase> GetActivePlayerShared() const;				// アクティブプレイヤー取得(shared_ptr版)
	PLAYER_TYPE GetActivePlayerType() const { return _activePlayerType; }	// アクティブプレイヤーのタイプ取得
	std::shared_ptr<PlayerBase> GetPlayerByType(PLAYER_TYPE type) const;	// タイプでプレイヤーを取得

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

protected:
	std::shared_ptr<CameraManager> _cameraManager;

	PlayerBase* _activePlayer;
	PLAYER_TYPE _activePlayerType;
	std::unordered_map<PLAYER_TYPE, std::shared_ptr<PlayerBase>> _players;

	bool _bEnableStateTransfer;		// プレイヤー切り替え時の状態引き継ぎフラグ

	// 入力関係
	int _key = 0;
	int _trg = 0;
	float _lx = 0.0f;
	float _ly = 0.0f;
	float _rx = 0.0f;
	float _ry = 0.0f;
	float _analogMin = 0.0f;

};

