// 担当 : 成田

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"

PlayerManager::PlayerManager()
{
	_activePlayer = nullptr;
	_activePlayerType = PLAYER_TYPE::SURFACE;

	_bEnableStateTransfer = true;
}

PlayerManager::~PlayerManager()
{
	_players.clear();
}

bool PlayerManager::Initialize()
{
	return true;
}

bool PlayerManager::Terminate()
{
	_players.clear();
	_activePlayer = nullptr;

	return true;
}

bool PlayerManager::Process()
{
	// プレイヤー切り替えの入力チェック
	if(_trg & PAD_INPUT_6) 
	{
		if(_activePlayerType == PLAYER_TYPE::SURFACE)
		{
			SwitchPlayer(PLAYER_TYPE::INTERIOR);
		}
		else
		{
			SwitchPlayer(PLAYER_TYPE::SURFACE);
		}
	}

	// アクティブプレイヤーの処理
	if(_activePlayer)
	{
		_activePlayer->SetInput(_key, _trg, _lx, _ly, _rx, _ry, _analogMin);
		_activePlayer->Process();
	}

	return true;
}

bool PlayerManager::Render()
{
	// アクティブプレイヤーのみ描画
	if(_activePlayer)
	{
		_activePlayer->Render();
	}

	return true;
}

// プレイヤーの登録関数
bool PlayerManager::RegisterPlayer(PLAYER_TYPE type, std::shared_ptr<PlayerBase> player)
{
	if(!player) return false;

	_players[type] = player;

	// 最初に登録されたプレイヤーをアクティブに設定
	if(!_activePlayer)
	{
		_activePlayer = player.get();
		_activePlayerType = type;
	}

	return true;
}

// プレイヤー切り替え関数
void PlayerManager::SwitchPlayer(PLAYER_TYPE type)
{
	// 指定されたタイプのプレイヤーが存在するかチェック
	auto it = _players.find(type);
	if(it == _players.end()) return;

	// 現在のプレイヤーと同じ場合は何もしない
	if(_activePlayerType == type) return;

	PlayerBase* oldPlayer = _activePlayer;
	PlayerBase* newPlayer = it->second.get();

	// 状態の引き継ぎ
	if(_bEnableStateTransfer && oldPlayer && newPlayer)
	{
		TransferPlayerState(oldPlayer, newPlayer);
	}

	// プレイヤーを切り替え
	_activePlayer = newPlayer;
	_activePlayer->Initialize();
	_activePlayerType = type;
}

// アクティブプレイヤー取得
PlayerBase* PlayerManager::GetActivePlayer() const
{
	return _activePlayer;
}

// アクティブプレイヤー取得（shared_ptr版）
std::shared_ptr<PlayerBase> PlayerManager::GetActivePlayerShared() const
{
	auto it = _players.find(_activePlayerType);
	if(it != _players.end())
	{
		return it->second;
	}

	return nullptr;
}

// タイプによって取得
std::shared_ptr<PlayerBase> PlayerManager::GetPlayerByType(PLAYER_TYPE type) const
{
	auto it = _players.find(type);
	if(it != _players.end())
	{
		return it->second;
	}

	return nullptr;
}

// 切り替え時の位置置き換え関数
void PlayerManager::TransferPlayerState(PlayerBase* from, PlayerBase* to)
{
	if(!from || !to) return;

	// 位置と向きの引き継ぎ
	to->SetPos(from->GetPos());
	to->SetDir(from->GetDir());

	// ステータスの引き継ぎ
	to->SetLife(from->GetLife());
}