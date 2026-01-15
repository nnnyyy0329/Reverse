// 担当 : 成田

#include "InteriorPlayer.h"

// デバッグ用表示
void InteriorPlayer::CallDraw()
{
	// モデルの表示
	DrawModel();

	_iDrawOffsetY = 0;	// 毎フレーム初期位置にリセット

	DrawBaseData();							// 基礎情報表示
	//DrawCoordinate();						// 座標関係の表示
	DrawCapsuleCollision();					// カプセルコリジョンを表示
	DrawStatus();							// ステータスを表示
	DrawParameter();						// パラメーター表示
	//DrawAnimationName();					// 再生されているアニメーション名表示
	DrawColPos();							// コリジョン情報表示
	_firstAttack.DrawAttackCollision();		// 第一攻撃コリジョン表示
	_secondAttack.DrawAttackCollision();	// 第二攻撃コリジョン表示
	_thirdAttack.DrawAttackCollision();		// 第三攻撃コリジョン表示

}

// モデルの表示
void InteriorPlayer::DrawModel()
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

// 基礎情報表示
void InteriorPlayer::DrawBaseData()
{
	// プレイヤーの名前表示
	{
		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "ーーーーーInteriorPlayerーーーーー");
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
	}
}

// 座標の表示
void InteriorPlayer::DrawCoordinate()
{
	// プレイヤーの座標情報
	{
		// 0,0,0を中心に線を引く
		{
			float linelength = 1000.f;
			VECTOR v = { 0, 0, 0 };
			DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
			DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
			DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
		}

		// プレイヤー位置情報表示
		{
			DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "Player:");
			_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

			DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z);
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
		case PLAYER_STATUS::FIRST_ATTACK:	// 攻撃1
			status = "FIRST_ATTACK";
			break;
		case PLAYER_STATUS::SECOND_ATTACK:	// 攻撃2
			status = "SECOND_ATTACK";
			break;
		case PLAYER_STATUS::THIRD_ATTACK:	// 攻撃3
			status = "THIRD_ATTACK";
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

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "Player Status: %s", status);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// パラメーター表示
void InteriorPlayer::DrawParameter()
{
	// 体力表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "Player Life: %f", _fLife);
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "Player Life: %f", _fLife);
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

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "Player AnimName: %d", _iAttachIndex);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// コリジョンの情報表示
void InteriorPlayer::DrawColPos()
{
	// プレイヤーのカプセルコリジョン情報
	{
		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "----Capsule Collision----");
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "  Top     = (%5.2f, %5.2f, %5.2f)", _vCollisionTop.x, _vCollisionTop.y, _vCollisionTop.z);
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "  Bottom  = (%5.2f, %5.2f, %5.2f)", _vCollisionBottom.x, _vCollisionBottom.y, _vCollisionBottom.z);
		_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

		DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, GetColor(0, 255, 0), "  Radius  = %5.2f", _fCollisionR);
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