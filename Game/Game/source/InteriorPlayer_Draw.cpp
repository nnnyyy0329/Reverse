// 担当 : 成田

#include "InteriorPlayer.h"

namespace color
{
	// 描画時の色
	constexpr int R = 255;
	constexpr int G = 255;
	constexpr int B = 255;
}

// 基礎情報表示
void InteriorPlayer::DrawBaseData()
{
	// プレイヤーの名前表示
	{
		// キャラタイプが一致したなら
		if(_eCharaType == CHARA_TYPE::INTERIOR_PLAYER)
		{
			DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "ーーーーーInteriorPlayerーーーーー");
			_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
		}
	}
}

// 座標の表示
void InteriorPlayer::DrawCoordinate()
{
	// プレイヤーの座標情報
	{
		// プレイヤー位置情報表示
		{
			DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "Player:");
			_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

			DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z);
			_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
		}
	}
}

// カプセルコリジョン表示
void InteriorPlayer::DrawCapsuleCollision()
{
	DrawCapsule3D
	(
		_vCollisionTop,
		_vCollisionBottom,
		_fCollisionR,
		8,
		GetColor(0, 255, 0),
		GetColor(0, 0, 255),
		TRUE
	);
}

// ステータス描画
void InteriorPlayer::DrawStatus()
{
	const char* status = "";
	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::NONE:			// 初期状態
			status = "NONE";
			break;
		case PLAYER_STATUS::WAIT:			// 待機
			status = "WAIT";
			break;
		case PLAYER_STATUS::WALK:			// 歩行
			status = "WALK";
			break;
		case PLAYER_STATUS::RUN:			// 走行
			status = "RUN";
			break;
		case PLAYER_STATUS::FIRST_ATTACK:	// 攻撃1
			status = "FIRST_ATTACK";
			break;
		case PLAYER_STATUS::SECOND_ATTACK:	// 攻撃2
			status = "SECOND_ATTACK";
			break;
		case PLAYER_STATUS::THIRD_ATTACK:	// 攻撃3
			status = "THIRD_ATTACK";
			break;
		case PLAYER_STATUS::FOURTH_ATTACK:	// 攻撃4
			status = "FOURTH_ATTACK";
			break;
		case PLAYER_STATUS::FIFTH_ATTACK:	// 攻撃5
			status = "FIFTH_ATTACK";
			break;
		case PLAYER_STATUS::JUMP_UP:		// ジャンプ上昇
			status = "JUMP_UP";
			break;
		case PLAYER_STATUS::JUMP_DOWN:		// ジャンプ下降
			status = "JUMP_DOWN";
			break;
		case PLAYER_STATUS::CROUCH_WAIT:	// しゃがみ待機
			status = "CROUCH_WAIT";
			break;
		case PLAYER_STATUS::CROUCH_WALK:	// しゃがみ歩行
			status = "CROUCH_WALK";
			break;
		case PLAYER_STATUS::DEATH:			// 死亡
			status = "DEATH";
			break;
		default:
			status = "UNKNOWN";
			break;
	}

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "Player Status: %s", status);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// パラメーター表示
void InteriorPlayer::DrawParameter()
{
	// 体力表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "Player Life: %f", _fLife);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// 再生されているアニメーション名表示
void InteriorPlayer::DrawAnimationName()
{
	const char* animName = "";

	switch(_ePlayerStatus)
	{
		case PLAYER_STATUS::WAIT:	// 待機
		{
			animName = "WAIT";
			break;
		}
		case PLAYER_STATUS::WALK:	// 歩行
		{
			animName = "WALK";
			break;
		}
		case PLAYER_STATUS::RUN:	// 走行
		{
			animName = "RUN";
			break;
		}
		case PLAYER_STATUS::JUMP_UP: // ジャンプ上昇
		{
			animName = "JUMP_UP";
			break;
		}
		case PLAYER_STATUS::JUMP_DOWN: // ジャンプ下降
		{
			animName = "JUMP_DOWN";
			break;
		}
		case PLAYER_STATUS::CROUCH_WAIT:	// しゃがみ待機
		{
			animName = "CROUCH_WAIT";
			break;
		}
		case PLAYER_STATUS::CROUCH_WALK:	// しゃがみ歩行
		{
			animName = "CROUCH_WALK";
			break;
		}
		case PLAYER_STATUS::FIRST_ATTACK:	// 攻撃1
		{
			animName = "FIRST_ATTACK";
			break;
		}
		case PLAYER_STATUS::SECOND_ATTACK:	// 攻撃2
		{
			animName = "SECOND_ATTACK";
			break;
		}
		case PLAYER_STATUS::THIRD_ATTACK:	// 攻撃3
		{
			animName = "THIRD_ATTACK";
			break;
		}
		case PLAYER_STATUS::DEATH:	// 死亡
		{
			animName = "DEATH";
			break;
		}
	}

	//DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "Player AnimName: %d", _iAttachIndex);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// コリジョンの情報表示
void InteriorPlayer::DrawColPos()
{
	// プレイヤーのカプセルコリジョン情報
	{
		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "----Capsule Collision----");
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "  Top     = (%5.2f, %5.2f, %5.2f)", _vCollisionTop.x, _vCollisionTop.y, _vCollisionTop.z);
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "  Bottom  = (%5.2f, %5.2f, %5.2f)", _vCollisionBottom.x, _vCollisionBottom.y, _vCollisionBottom.z);
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(color::R, color::G, color::B), "  Radius  = %5.2f", _fCollisionR);
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
	}
}

// 受け取ったコリジョンのデータ表示
void InteriorPlayer::DrawAttackColData()
{
	// 攻撃コリジョン情報のタイトル表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(255, 255, 0), "----Attack Collision Data----");
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす


	// 受け取ったコリジョンデータの表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(255, 255, 0), "  Col Top    = (%f, %f, %f)", _vAttackColTop.x, _vAttackColTop.y, _vAttackColTop.z);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(255, 255, 0), "  Col Bottom = (%5.2f, %5.2f, %5.2f)", _vAttackColBottom.x, _vAttackColBottom.y, _vAttackColBottom.z);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(255, 255, 0), "  Col Radius = %5.2f", _fAttackColR);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	// コンボ情報の表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(255, 255, 0), "  Combo Count: %d", _iComboCount);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}