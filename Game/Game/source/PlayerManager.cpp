// 担当 : 成田

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"
#include "EnergyManager.h"

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
	// 入力によるプレイヤー切り替え
	//SwitchPlayerByInput();

	// エネルギーによるプレイヤー切り替え
	SwitchPlayerByEenrgy();

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

// 入力によるプレイヤー切り替え
void PlayerManager:: SwitchPlayerByInput()
{
	// プレイヤーの入力チェック
	if(_trg & PAD_INPUT_6)
	{
		// エネルギーのインスタンス取得
		auto* energyManager = EnergyManager::GetInstance();

		// 表プレイヤーで切り替え可能なら
		if(_activePlayerType == PLAYER_TYPE::SURFACE && energyManager->CanSwitchPlayer()) 
		{
			// 表プレイヤー以外に切り替え
			SwitchPlayer(PLAYER_TYPE::BULLET);
			//SwitchPlayer(PLAYER_TYPE::INTERIOR);
		}
		// 裏プレイヤーなら表プレイヤーに切り替え
		else if(_activePlayerType != PLAYER_TYPE::SURFACE)
		{
			// 裏プレイヤーから表プレイヤーに切り替え
			SwitchPlayer(PLAYER_TYPE::SURFACE);
		}
	}
}

// アビリティによるプレイヤー切り替え
void PlayerManager:: SwitchPlayerByAbility(ABILITY_TYPE ability)
{
	// 切り替え対象プレイヤータイプ
	PLAYER_TYPE targetType = PLAYER_TYPE::NONE;

	// アビリティに応じてプレイヤー切り替え
	switch(ability)
	{
		case ABILITY_TYPE::SURFACE_PLAYER: // 表プレイヤー
		{
			targetType = PLAYER_TYPE::SURFACE;
			break;
		}

		case ABILITY_TYPE::INTERIOR_PLAYER: // 裏プレイヤー
		{
			targetType = PLAYER_TYPE::INTERIOR;
			break;
		}

		case ABILITY_TYPE::BULLET_PLAYER: // 弾プレイヤー
		{
			targetType = PLAYER_TYPE::BULLET;
			break;
		}

		default:
			break;
	}

	// 切り替え対象があるなら
	if(targetType != PLAYER_TYPE::NONE)
	{
		// プレイヤー切り替え
		SwitchPlayer(targetType);
	}
}

// エネルギーによるプレイヤー切り替え
void PlayerManager:: SwitchPlayerByEenrgy()
{
	// エネルギーのインスタンス取得
	auto* energyManager = EnergyManager::GetInstance();

	// 表プレイヤー以外がアクティブで、エネルギー不足なら
	if(_activePlayerType != PLAYER_TYPE::SURFACE && !energyManager->CanKeepSwitchPlayer()) 
	{
		// 強制的に表プレイヤーに切り替え
		SwitchPlayer(PLAYER_TYPE::SURFACE);
	}
}

// プレイヤー切り替え関数
void PlayerManager::SwitchPlayer(PLAYER_TYPE type)
{
	// 指定されたタイプのプレイヤーが存在するかチェック
	auto player = _players.find(type);
	if(player == _players.end()){ return; }

	// 現在のプレイヤーと同じ場合は何もしない
	if(_activePlayerType == type){ return; }

	PlayerBase* oldPlayer = _activePlayer;			// 古いプレイヤー
	PlayerBase* newPlayer = player->second.get();	// 新しいプレイヤー

	// プレイヤーを切り替え
	_activePlayer = newPlayer;		// 新しいアクティブプレイヤーに設定
	_activePlayerType = type;		// アクティブプレイヤータイプを更新

	// 状態の引き継ぎ
	if(_bEnableStateTransfer && oldPlayer && newPlayer)
	{
		// 位置と状態の引き継ぎ
		TransferPlayerState(oldPlayer, newPlayer);
	}
}

// アクティブプレイヤー取得
PlayerBase* PlayerManager::GetActivePlayer() const
{
	return _activePlayer;	// 生ポインタを返す
}

// アクティブプレイヤー取得（shared_ptr版）
std::shared_ptr<PlayerBase> PlayerManager::GetActivePlayerShared() const
{
	auto player = _players.find(_activePlayerType);

	// 見つかったら
	if(player != _players.end())
	{
		return player->second;	// shared_ptrを返す
	}

	return nullptr;
}

// タイプによって取得
std::shared_ptr<PlayerBase> PlayerManager::GetPlayerByType(PLAYER_TYPE type) const
{
	auto player = _players.find(type);

	// 見つかったら
	if(player != _players.end())
	{
		return player->second;	// shared_ptrを返す
	}

	return nullptr;
}

// 切り替え時の位置置き換え関数
void PlayerManager::TransferPlayerState(PlayerBase* oldPlayer, PlayerBase* newPlayer)
{
	if(!oldPlayer || !newPlayer) return;

	// 位置と向きの引き継ぎ
	newPlayer->SetPos(oldPlayer->GetPos());
	newPlayer->SetDir(oldPlayer->GetDir());

	// ステータスの引き継ぎ
	newPlayer->SetLife(oldPlayer->GetLife());
}