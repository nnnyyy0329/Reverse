// 担当 : 成田

#include "PlayerBase.h"

// デバッグ表示関数
void PlayerBase::DebugRender()
{
	// デバッグ描画設定取得
	_renderConfig = GetRenderConfig();

	_iDrawOffsetY = 0;	// 毎フレーム初期位置にリセット

	DrawBaseData();		// 基礎情報表示
	DrawCoordinate();	// 座標関係の表示
	DrawStatus();		// ステータスを表示
	DrawParameter();	// パラメーター表示
	DrawColPos();		// コリジョン情報表示
}

// 基礎情報表示
void PlayerBase::DrawBaseData()
{
	auto color = GetColor(_renderConfig.debugColor.r, _renderConfig.debugColor.g, _renderConfig.debugColor.b);

	// プレイヤーの名前表示
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,"ーーーーー%sーーーーー", _renderConfig.playerName);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// 座標の表示
void PlayerBase::DrawCoordinate()
{
	auto color = GetColor(_renderConfig.debugColor.r, _renderConfig.debugColor.g, _renderConfig.debugColor.b);

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color, "Player:");
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,"pos = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// ステータス描画
void PlayerBase::DrawStatus()
{
	auto color = GetColor(_renderConfig.debugColor.r, _renderConfig.debugColor.g, _renderConfig.debugColor.b);

	// ステータス文字列取得関数
	const char* statusStr = GetStatusString(_ePlayerStatus);

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,"Player Status: %s", statusStr);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// パラメーター表示
void PlayerBase::DrawParameter()
{
	auto color = GetColor(_renderConfig.debugColor.r, _renderConfig.debugColor.g, _renderConfig.debugColor.b);

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,"Player Life: %.2f/%.2f", _fLife, _fMaxLife);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// コリジョンの情報表示
void PlayerBase::DrawColPos()
{
	auto color = GetColor(_renderConfig.debugColor.r, _renderConfig.debugColor.g, _renderConfig.debugColor.b);

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color, "----Capsule Collision----");
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,
		"  Top     = (%5.2f, %5.2f, %5.2f)", _vCollisionTop.x, _vCollisionTop.y, _vCollisionTop.z);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
	
	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,
		"  Bottom  = (%5.2f, %5.2f, %5.2f)", _vCollisionBottom.x, _vCollisionBottom.y, _vCollisionBottom.z);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color,
		"  Radius  = %5.2f", _fCollisionR);
	_iDrawOffsetY += _iDrawSizeOffset;	// 表示位置をずらす
}

// ステータス文字列取得関数
const char* PlayerBase::GetStatusString(PLAYER_STATUS status)
{
	switch(status)
	{
		case PLAYER_STATUS::NONE:			return "NONE";			// 初期
		case PLAYER_STATUS::WAIT:			return "WAIT";			// 待機
		case PLAYER_STATUS::WALK:			return "WALK";			// 歩行
		case PLAYER_STATUS::RUN:			return "RUN";			// 走行
		case PLAYER_STATUS::FIRST_ATTACK:	return "FIRST_ATTACK";	// 1段目攻撃
		case PLAYER_STATUS::SECOND_ATTACK:	return "SECOND_ATTACK";	// 2段目攻撃
		case PLAYER_STATUS::THIRD_ATTACK:	return "THIRD_ATTACK";	// 3段目攻撃
		case PLAYER_STATUS::FOURTH_ATTACK:	return "FOURTH_ATTACK";	// 4段目攻撃
		case PLAYER_STATUS::FIFTH_ATTACK:	return "FIFTH_ATTACK";	// 5段目攻撃
		case PLAYER_STATUS::JUMP_UP:		return "JUMP_UP";		// ジャンプ（上昇）
		case PLAYER_STATUS::JUMP_DOWN:		return "JUMP_DOWN";		// ジャンプ（下降）
		case PLAYER_STATUS::CROUCH_WAIT:	return "CROUCH_WAIT";	// しゃがみ待機
		case PLAYER_STATUS::CROUCH_WALK:	return "CROUCH_WALK";	// しゃがみ歩行
		case PLAYER_STATUS::HIT:			return "HIT";			// 被弾
		case PLAYER_STATUS::DODGE:			return "DODGE";			// 回避
		case PLAYER_STATUS::DEATH:			return "DEATH";			// 死亡
		default:							return "UNKNOWN";		// 不明
	}
}