// 担当 : 成田

#include "PlayerManager.h"
#include "SurfacePlayer.h"
#include "InteriorPlayer.h"
#include "EnergyManager.h"
#include "CameraManager.h"
#include "AbilitySelectScreen.h"

// 変身設定用の名前空間
namespace TransformContants
{
	constexpr float TRANSFORM_TIME_INCREMENT = 1.0f;	// 変身時間の増加量
	constexpr float ANIMATION_BLEND_TIME = 0.5f;		// アニメーションブレンド時間
	constexpr int ANIMATION_NONE_LOOP_COUNT = 0;		// ループなしのループカウント
	constexpr int ANIMATION_LOOP_COUNT = 1;				// ループありのループカウント
}

PlayerManager::PlayerManager()
{
	_cameraManager = nullptr;		// カメラマネージャー
	_abilitySelectScreen = nullptr;	// アビリティ選択画面

	_activePlayer = nullptr;
	_eActivePlayerType = PLAYER_TYPE::SURFACE;

	_eTransformTarget = PLAYER_TYPE::NONE;
	_fTransformTime = 0.0f;
	_fTransformMaxTime = 0.0f;
	_bIsTransforming = false;

	_fTransformCancelTime = 0.0f;
	_fTransformCancelMaxTime = 0.0f;
	_bIsTransformCanceling = false;

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
	// 変身処理
	UpdateTransform();

	// 変身解除処理
	UpdateTransformCancel();

	// 能力選択画面がアクティブかチェック
	bool isAbilityScreenActive = false;
	CheckAbilityScreenActive(isAbilityScreenActive);

	// 変身中でなければ自動切り替えなどの処理
	ProcessAutoSwitch(isAbilityScreenActive);

	// アクティブプレイヤーの処理
	ProcessActivePlayer(isAbilityScreenActive);

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

// デバッグ処理
bool PlayerManager::DebugProcess()
{


	return true;
}

// デバッグ描画
bool PlayerManager::DebugRender()
{
	// 変身時間の描画
	if(_bIsTransforming)
	{
		std::string debugText = "Transforming: " + std::to_string(_fTransformTime) + " / " + std::to_string(_fTransformMaxTime);
		DrawString(10, 310, debugText.c_str(), GetColor(255, 255, 255));
	}

	// 変身解除時間の描画
	if(_bIsTransformCanceling)
	{
		std::string debugText = "Transform Canceling: " + std::to_string(_fTransformCancelTime) + " / " + std::to_string(_fTransformCancelMaxTime);
		DrawString(10, 330, debugText.c_str(), GetColor(255, 255, 255));
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
		_eActivePlayerType = type;
	}

	return true;
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
void PlayerManager:: SwitchPlayerByEnergy()
{
	// エネルギーのインスタンス取得
	auto* energyManager = EnergyManager::GetInstance();

	// 表プレイヤー以外がアクティブで、エネルギー不足なら
	if(_eActivePlayerType != PLAYER_TYPE::SURFACE && !energyManager->CanKeepSwitchPlayer())
	{
		// 強制的に表プレイヤーに切り替え
		SwitchPlayer(PLAYER_TYPE::SURFACE);

		// 変身中なら変身を中断して解除開始
		if(_bIsTransforming)
		{
			// 変身中断
			EndTransform();
		}

		// 変身解除開始
		StartTransformCancel();
	}
}

// プレイヤー切り替え関数
void PlayerManager::SwitchPlayer(PLAYER_TYPE type)
{
	// 指定されたタイプのプレイヤーが存在するかチェック
	auto player = _players.find(type);
	if(player == _players.end()){ return; }

	// 現在のプレイヤーと同じ場合は何もしない
	if(_eActivePlayerType == type){ return; }

	// 変身開始
	StartTransform(type);
}

// 変身開始
void PlayerManager::StartTransform(PLAYER_TYPE targetType)
{
	// 表プレイヤーへの切り替えは即時切り替え
	if(targetType == PLAYER_TYPE::SURFACE)
	{
		// 即時切り替え
		//SwitchPlayerImmediate(targetType);

		return;
	}

	_eTransformTarget = targetType;	// 変身先のプレイヤーをセット
	_fTransformTime = 0.0f;			// 変身時間リセット

	// 変身ステートの適応
	if(_activePlayer)
	{
		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::TRANSFORM);
	}

	// 変身アニメーション再生
	PlayerAnimations playerAnim = _activePlayer->GetPlayerAnimation();	// プレイヤーのアニメーションデータを取得
	PlayTransConnectionAnim(playerAnim.combat.transform);				// 再生処理

	_bIsTransforming = true;	// 変身フラグ有効
}

// 即時変身
void PlayerManager::SwitchPlayerImmediate(PLAYER_TYPE targetType)
{
	auto player = _players.find(targetType);
	if(player == _players.end()){ return; }	// プレイヤーが存在しないなら
	
	if(_eActivePlayerType == targetType){ return; }	// 現在のタイプと次のタイプが一致しないならスキップ

	PlayerBase* oldPlayer = _activePlayer;			// 古いプレイヤー
	PlayerBase* newPlayer = player->second.get();	// 新しいプレイヤー

	// プレイヤーを切り替え
	_activePlayer = newPlayer;									// 有効プレイヤーを新しいプレイヤーにする
	_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);	// 特殊状態をリセット
	_eActivePlayerType = targetType;							// アクティブプレイヤータイプを変更

	// 状態の引き継ぎ
	if(_bEnableStateTransfer && oldPlayer && newPlayer)
	{
		// 位置と状態の引き継ぎ
		TransferPlayerState(oldPlayer, newPlayer);
	}
}

// 変身処理
void PlayerManager::UpdateTransform()
{
	if(!_bIsTransforming){ return; }	// 変身中でなければ何もしない

	// 変身タイマー更新処理
	UpdateTransformTime();

	// 変身時間による変身終了処理
	TransformFinishByTime();

	// 変身時間が最大を超えたら変身終了
	if(_fTransformTime >= _fTransformMaxTime)
	{
		// 変身終了
		EndTransform();
	}
}

// 変身時間による変身終了処理
void PlayerManager::TransformFinishByTime()
{
	// 変身時間が最大を超えいて、現在のタイプと変身先のタイプが不一致なら変身終了
	if((_fTransformTime >= _fTransformMaxTime) && (_eActivePlayerType != _eTransformTarget))
	{
		// プレイヤー切り替え
		PlayerBase* oldPlayer = _activePlayer;				// 古いプレイヤー
		auto newPlayer = _players.find(_eTransformTarget);	// 新しいプレイヤー

		// 存在チェック
		if(newPlayer != _players.end())
		{
			_activePlayer = newPlayer->second.get();	// 新しいアクティブプレイヤーに設定
			_eActivePlayerType = _eTransformTarget;		// アクティブプレイヤータイプを更新

			// 状態の引き継ぎ
			if(_bEnableStateTransfer && oldPlayer && _activePlayer)
			{
				// 位置と状態の引き継ぎ
				TransferPlayerState(oldPlayer, _activePlayer);
			}
		}
	}
}

// 変身終了
void PlayerManager::EndTransform()
{   
	// 戦闘状態を通常に戻す
    if(_activePlayer)
    {
        _activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
    }

	_eTransformTarget = PLAYER_TYPE::NONE;
	_fTransformTime = 0.0f;
	_bIsTransforming = false;
}

// 変身解除開始
void PlayerManager::StartTransformCancel()
{
	if(_bIsTransformCanceling){ return; }	// すでに変身解除中なら何もしない

	if(_eActivePlayerType == PLAYER_TYPE::SURFACE){ return; }	// 表プレイヤーなら何もしない

	_fTransformCancelTime = 0.0f;

	// 変身解除ステートの適応
	if(_activePlayer)
	{
		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::TRANS_CANCEL); 
	}

	// 変身解除アニメーション再生
	PlayerAnimations playerAnim = _activePlayer->GetPlayerAnimation();	// プレイヤーのアニメーションデータを取得
	PlayTransConnectionAnim(playerAnim.combat.transCancel);				// 再生処理

	_bIsTransformCanceling = true;
}

// 変身解除処理
void PlayerManager::UpdateTransformCancel()
{
	if(!_bIsTransformCanceling){ return; }

	// 変身解除タイマー更新処理
	UpdateTransformCancelTime();

	// 時間によるプレイヤー切り替え処理
	SwitchPlayerByTime();
}

// 時間によるプレイヤー切り替え処理
void PlayerManager::SwitchPlayerByTime()
{
	// 変身解除時間が最大を超えたら表プレイヤーに切り替え
	if(_fTransformCancelTime >= _fTransformCancelMaxTime)
	{
		// 表プレイヤーに即座に切り替え
		if(_eActivePlayerType != PLAYER_TYPE::SURFACE)
		{
			// 即時変身
			SwitchPlayerImmediate(PLAYER_TYPE::SURFACE);
		}

		// 変身解除終了
		EndTransformCancel();
	}
}

// 変身解除終了
void PlayerManager::EndTransformCancel()
{
	// 戦闘状態を通常に戻す
	if(_activePlayer)
	{
		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
	}

	_fTransformCancelTime = 0.0f;
	_bIsTransformCanceling = false;
}

// アクティブプレイヤー取得
PlayerBase* PlayerManager::GetActivePlayer() const
{
	return _activePlayer;	// 生ポインタを返す
}

// アクティブプレイヤー取得（shared_ptr版）
std::shared_ptr<PlayerBase> PlayerManager::GetActivePlayerShared() const
{
	auto player = _players.find(_eActivePlayerType);

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

	// 新しいプレイヤーの状態をリセット
	newPlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
	newPlayer->SetMovementState(PLAYER_MOVEMENT_STATE::WAIT);

	// 待機アニメーション再生
	PlayerAnimations playerAnim = _activePlayer->GetPlayerAnimation();	// プレイヤーのアニメーションデータを取得
	PlayTransConnectionAnim(playerAnim.movement.wait);					// 再生処理

	// プレイヤー切り替え時にエイムモードが終了していない場合があるため、強制的にエイムモードを終了する
	_cameraManager->EndAimMode();
}

// 変身タイマー更新処理
void PlayerManager::UpdateTransformTime()
{
	_fTransformTime += 1.0f;	// 変身時間を更新
}

// 変身解除タイマー更新処理
void PlayerManager::UpdateTransformCancelTime()
{
	_fTransformCancelTime += 1.0f;	// 変身解除時間を更新
}

// 変身関連アニメーション再生関数
void PlayerManager::PlayTransConnectionAnim(const char* animName)
{
	if(!_activePlayer) { return; }

	// アニメーションマネージャーの取得
	auto* animManager = _activePlayer->GetAnimManager();	// 有効なプレイヤーからアニメ管理クラス取得
	if(!animManager){ return; }								// アニメ管理クラスがないならスキップ

	if(animName != nullptr)
	{
		// 変身アニメーションの再生
		animManager->ChangeAnimationByName(animName, 0.0f, 1);			// アニメーション変更
		_fTransformMaxTime = animManager->GetCurrentAnimTotalTime();	// 変身アニメーションの再生時間を変身時間に設定
	}
}

// 能力選択画面が存在するかをチェック
void PlayerManager::CheckAbilityScreenActive(bool& isAbilityScreenActive)
{
	isAbilityScreenActive = false; // 存在するかのフラグ

	// 能力選択画面クラス有効チェック
	if(_abilitySelectScreen)
	{
		// 有効なら存在を有効にする
		isAbilityScreenActive = _abilitySelectScreen->GetIsScreenActive();
	}
}

// 自動切り替え系（エネルギーによる強制切り替えなど）をまとめる
void PlayerManager::ProcessAutoSwitch(bool isAbilityScreenActive)
{
	// 変身中でなければプレイヤー切り替え処理
	if(!_bIsTransforming && !isAbilityScreenActive)
	{
		// エネルギーによるプレイヤー切り替え
		SwitchPlayerByEnergy();
	}
}

// アクティブプレイヤーへの処理
void PlayerManager::ProcessActivePlayer(bool isAbilityScreenActive)
{
	if(!_activePlayer) { return; }

	auto im = InputManager::GetInstance();

	// 変身中、または変身解除中、またはアビリティ選択画面がアクティブな場合
	if(_bIsTransforming || _bIsTransformCanceling || isAbilityScreenActive)
	{
		// ここで入力の停止処理を追加

	}
	else
	{
		// ここで入力をアクティブプレイヤーに渡す

	}

	_activePlayer->SetCameraManager(_cameraManager);	// アクティブプレイヤーにカメラマネージャーをセット
	_activePlayer->Process();							// アクティブプレイヤーの処理を行う
}
