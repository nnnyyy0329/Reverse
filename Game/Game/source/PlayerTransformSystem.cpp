#include "PlayerTransformSystem.h"

//// 変身開始
//void PlayerTransformSystem::StartTransform(PLAYER_TYPE targetType)
//{
//	// 表プレイヤーへの切り替えは即時切り替え
//	if(targetType == PLAYER_TYPE::SURFACE)
//	{
//		// 即時切り替え
//		SwitchPlayerImmediate(targetType);
//
//		return;
//	}
//
//	_eTransformTarget = targetType;
//	_fTransformTime = 0.0f;
//
//	// 変身ステートの適応
//	if(_activePlayer)
//	{
//		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::TRANSFORM);
//	}
//
//	// アニメーションマネージャーの取得
//	auto* animManager = _activePlayer->GetAnimManager();
//	if(!animManager){ return; }	// アニメーションマネージャーがないならスキップ
//
//	// 変身アニメーションの再生
//	PlayerAnimations playerAnims = _activePlayer->GetPlayerAnimation();	// プレイヤーのアニメーションデータを取得
//	const char* transformAnimName = playerAnims.combat.transform;		// 変身アニメーション名を取得
//	if(transformAnimName != nullptr)
//	{
//		// 変身アニメーションの再生
//		animManager->ChangeAnimationByName(transformAnimName, 0.0f, 1);	// アニメーション変更
//		_fTransformMaxTime = animManager->GetCurrentAnimTotalTime();	// 変身アニメーションの再生時間を変身時間に設定
//	}
//
//	_bIsTransforming = true;
//}
//
//// 即時変身
//void PlayerTransformSystem::SwitchPlayerImmediate(PLAYER_TYPE targetType)
//{
//	auto player = _players.find(targetType);
//	if(player == _players.end()){ return; }
//
//	if(_eActivePlayerType == targetType){ return; }
//
//	PlayerBase* oldPlayer = _activePlayer;
//	PlayerBase* newPlayer = player->second.get();
//
//	// プレイヤーを切り替え
//	_activePlayer = newPlayer;
//	_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
//	_eActivePlayerType = targetType;
//
//	// 状態の引き継ぎ
//	if(_bEnableStateTransfer && oldPlayer && newPlayer)
//	{
//		TransferPlayerState(oldPlayer, newPlayer);
//	}
//}
//
//// 変身処理
//void PlayerTransformSystem::ProcessTransform()
//{
//	if(!_bIsTransforming){ return; }
//
//	_fTransformTime += 1.0f;	// 変身時間を更新
//
//	// 変身時間が最大を超えたら変身終了
//	if(_fTransformTime >= _fTransformMaxTime && _eActivePlayerType != _eTransformTarget)
//	{
//		// プレイヤー切り替え
//		PlayerBase* oldPlayer = _activePlayer;				// 古いプレイヤー
//		auto newPlayer = _players.find(_eTransformTarget);	// 新しいプレイヤー
//		if(newPlayer != _players.end())
//		{
//			_activePlayer = newPlayer->second.get();	// 新しいアクティブプレイヤーに設定
//			_eActivePlayerType = _eTransformTarget;		// アクティブプレイヤータイプを更新
//
//			// 状態の引き継ぎ
//			if(_bEnableStateTransfer && oldPlayer && _activePlayer)
//			{
//				// 位置と状態の引き継ぎ
//				TransferPlayerState(oldPlayer, _activePlayer);
//			}
//		}
//	}
//
//	// 変身時間が最大を超えたら変身終了
//	if(_fTransformTime >= _fTransformMaxTime)
//	{
//		// 変身終了
//		EndTransform();
//	}
//}
//
//// 変身終了
//void PlayerTransformSystem::EndTransform()
//{
//	// 戦闘状態を通常に戻す
//	if(_activePlayer)
//	{
//		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
//	}
//
//	_eTransformTarget = PLAYER_TYPE::NONE;
//	_fTransformTime = 0.0f;
//	_bIsTransforming = false;
//}
