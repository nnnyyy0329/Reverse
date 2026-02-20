#include "PlayerTransformCancelSystem.h"

//// 変身解除開始
//void PlayerTransformCancelSystem::StartTransformCancel()
//{
//	if(_bIsTransformCanceling){ return; }	// すでに変身解除中なら何もしない
//
//	if(_eActivePlayerType == PLAYER_TYPE::SURFACE){ return; }	// 表プレイヤーなら何もしない
//
//	_fTransformCancelTime = 0.0f;
//
//	// 変身解除ステートの適応
//	if(_activePlayer)
//	{
//		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::TRANS_CANCEL);
//	}
//
//	// アニメーションマネージャーの取得
//	auto* animManager = _activePlayer->GetAnimManager();
//	if(!animManager){ return; }	// アニメーションマネージャーがないならスキップ
//
//	// 変身アニメーションの再生
//	PlayerAnimations playerAnims = _activePlayer->GetPlayerAnimation();	// プレイヤーのアニメーションデータを取得
//	const char* transformAnimName = playerAnims.combat.transCancel;		// 変身アニメーション名を取得
//	if(transformAnimName != nullptr)
//	{
//		// 変身アニメーションの再生
//		animManager->ChangeAnimationByName(transformAnimName, 0.0f, 1);		// アニメーション変更
//		_fTransformCancelMaxTime = animManager->GetCurrentAnimTotalTime();	// 変身アニメーション再生時間を変身時間に設定
//	}
//
//	_bIsTransformCanceling = true;
//}
//
//// 変身解除処理
//void PlayerTransformCancelSystem::ProcessTransformCancel()
//{
//	if(!_bIsTransformCanceling){ return; }
//
//	_fTransformCancelTime += 1.0f;	// 変身解除時間を更新
//
//	// 変身解除時間が最大を超えたら表プレイヤーに切り替え
//	if(_fTransformCancelTime >= _fTransformCancelMaxTime)
//	{
//		//// アクティブプレイヤーが表プレイヤー以外なら
//		//if(_eActivePlayerType != PLAYER_TYPE::SURFACE)
//		//{
//		//	// プレイヤー切り替え
//		//	PlayerBase* oldPlayer = _activePlayer;				// 古いプレイヤー
//		//	auto newPlayer = _players.find(_eTransformTarget);	// 新しいプレイヤー
//		//	if(newPlayer != _players.end())
//		//	{
//		//		_activePlayer = newPlayer->second.get();	// 新しいアクティブプレイヤーに設定
//		//		_eActivePlayerType = _eTransformTarget;		// アクティブプレイヤータイプを更新
//
//		//		// 状態の引き継ぎ
//		//		if(_bEnableStateTransfer && oldPlayer && _activePlayer)
//		//		{
//		//			// 位置と状態の引き継ぎ
//		//			TransferPlayerState(oldPlayer, _activePlayer);
//		//		}
//		//	}
//
//		//	// プレイヤー切り替え
//		//	SwitchPlayer(PLAYER_TYPE::SURFACE);
//		//}
//
//		// 表プレイヤーに即座に切り替え
//		if(_eActivePlayerType != PLAYER_TYPE::SURFACE)
//		{
//			SwitchPlayerImmediate(PLAYER_TYPE::SURFACE);
//		}
//
//		// 変身解除終了
//		EndTransformCancel();
//	}
//}
//
//// 変身解除終了
//void PlayerTransformCancelSystem::EndTransformCancel()
//{
//	// 戦闘状態を通常に戻す
//	if(_activePlayer)
//	{
//		_activePlayer->SetCombatState(PLAYER_COMBAT_STATE::NONE);
//	}
//
//	_fTransformCancelTime = 0.0f;
//	_bIsTransformCanceling = false;
//}
