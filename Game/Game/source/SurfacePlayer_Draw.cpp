#include "SurfacePlayer.h"

// モデルの描画
void SurfacePlayer::DrawModel()
{
	// 再生時間をセットする
	MV1SetAttachAnimTime(_iHandle, _iAttachIndex, _fPlayTime);

	// モデルを描画する
	{
		// 位置
		MV1SetPosition(_iHandle, _vPos);

		// 向きからY軸回転を算出
		VECTOR vRot = { 0,0,0 };
		vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);	// モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
		MV1SetRotationXYZ(_iHandle, vRot);

		// 描画
		MV1DrawModel(_iHandle);

		// コリジョン判定用ラインの描画
		if(_bViewCollision)
		{
			DrawLine3D(VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)), GetColor(255, 0, 0));
		}
	}
}

/* ーーーーーーーーーーーーーーーーーーデバッグ表示ーーーーーーーーーーーーーーーーーー */

// その他の描画
void SurfacePlayer::DrawOther()
{
	// いったんとりあえずこれ
	{
		// 0,0,0を中心に線を引く
		{
			float linelength = 1000.f;
			VECTOR v = { 0, 0, 0 };
			DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
			DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
			DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
		}

		// プレイヤー情報表示
		{
			int x = 0, y = 0, size = 16;
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 0, 0), "Player:"); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
		}
	}
}

// カプセルコリジョン描画
void SurfacePlayer::DrawCapsuleCollision()
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

	// カプセルコリジョン座標表示
	{
		int x = 400, y = 0, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(255, 0, 0), "Capsule Collision:"); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Top     = (%5.2f, %5.2f, %5.2f)", _vCollisionTop.x, _vCollisionTop.y, _vCollisionTop.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Bottom  = (%5.2f, %5.2f, %5.2f)", _vCollisionBottom.x, _vCollisionBottom.y, _vCollisionBottom.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  Radius  = %5.2f", _fCollisionR); y += size;
	}
}

// ステータス描画
void SurfacePlayer::DrawStatus()
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
		case PLAYER_STATUS::FIRST_ATTACK:	// 攻撃1
			status = "FIRST_ATTACK";
			break;
		case PLAYER_STATUS::SECOND_ATTACK:	// 攻撃2
			status = "SECOND_ATTACK";
			break;
		case PLAYER_STATUS::THIRD_ATTACK:	// 攻撃3
			status = "THIRD_ATTACK";
			break;
		case PLAYER_STATUS::JUMP_UP:			// ジャンプ上昇
			status = "JUMP_UP";
			break;
		case PLAYER_STATUS::JUMP_DOWN:		// ジャンプ下降
			status = "JUMP_DOWN";
			break;
		case PLAYER_STATUS::CROUCH_WAIT:		// しゃがみ待機
			status = "CROUCH_WAIT";
			break;
		case PLAYER_STATUS::CROUCH_WALK:		// しゃがみ歩行
			status = "CROUCH_WALK";
			break;
		case PLAYER_STATUS::DEATH:			// 死亡
			status = "DEATH";
			break;
		default:
			status = "UNKNOWN";
			break;
	}

	DrawFormatString(0, 40, GetColor(255, 0, 0), "Player Status: %s", status);
}