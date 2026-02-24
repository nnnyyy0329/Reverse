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
	std::string statusStr = GetCurrentStateString();

	DrawFormatString(_iDrawOffsetX, _iDrawOffsetY, color, "Player Status: %s", statusStr.c_str());
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

// 現在のステータス文字列取得関数
std::string PlayerBase::GetCurrentStateString() const
{
	std::string result;

	// 移動状態
	result += "Move:" + GetMovementStateString(_playerState.movementState);

	// 攻撃状態
	if(_playerState.attackState != PLAYER_ATTACK_STATE::NONE)
	{
		result += " | Attack:" + GetAttackStateString(_playerState.attackState);
	}

	// 発射状態
	if(_playerState.shootState != PLAYER_SHOOT_STATE::NONE)
	{
		result += " | Shoot:" + GetShootStateString(_playerState.shootState);
	}

	// 吸収攻撃状態
	if(_playerState.absorbState != PLAYER_ABSORB_STATE::NONE)
	{
		result += " | Absorb:" + GetAbsorbStateString(_playerState.absorbState);
	}

	// 特殊状態
	if(_playerState.combatState != PLAYER_COMBAT_STATE::NONE)
	{
		result += " | Combat:" + GetCombatStateString(_playerState.combatState);
	}

	// 最終的な文字列を返す
	return result.c_str();
}

// 基本移動状態文字列取得
std::string PlayerBase::GetMovementStateString(PLAYER_MOVEMENT_STATE state) const
{
	switch(state)
	{
		case PLAYER_MOVEMENT_STATE::NONE:        return "NONE";
		case PLAYER_MOVEMENT_STATE::WAIT:        return "WAIT";			// 待機
		case PLAYER_MOVEMENT_STATE::WALK:        return "WALK";			// 歩き
		case PLAYER_MOVEMENT_STATE::RUN:         return "RUN";			// 走り
		case PLAYER_MOVEMENT_STATE::JUMP_UP:     return "JUMP_UP";		// ジャンプ上昇
		case PLAYER_MOVEMENT_STATE::JUMP_DOWN:   return "JUMP_DOWN";	// ジャンプ下降
		case PLAYER_MOVEMENT_STATE::CROUCH_WAIT: return "CROUCH_WAIT";	// しゃがみ待機
		case PLAYER_MOVEMENT_STATE::CROUCH_WALK: return "CROUCH_WALK";	// しゃがみ歩き
		default:                                 return "UNKNOWN";
	}
}

// 攻撃状態文字列取得
std::string PlayerBase::GetAttackStateString(PLAYER_ATTACK_STATE state) const
{
	switch(state)
	{
		case PLAYER_ATTACK_STATE::NONE:					return "NONE";
		case PLAYER_ATTACK_STATE::FIRST_ATTACK:			return "FIRST_ATTACK";		// 1段目攻撃
		case PLAYER_ATTACK_STATE::SECOND_ATTACK:		return "SECOND_ATTACK";		// 2段目攻撃
		case PLAYER_ATTACK_STATE::THIRD_ATTACK:			return "THIRD_ATTACK";		// 3段目攻撃
		case PLAYER_ATTACK_STATE::FOURTH_ATTACK:		return "FOURTH_ATTACK";		// 4段目攻撃
		case PLAYER_ATTACK_STATE::FIFTH_ATTACK:			return "FIFTH_ATTACK";		// 5段目攻撃
		case PLAYER_ATTACK_STATE::FIRST_SKILL:			return "FIRST_SKILL";		// スキル1
		case PLAYER_ATTACK_STATE::SECOND_SKILL:			return "SECOND_SKILL";		// スキル2
		default:										return "UNKNOWN";
	}
}

// 弾発射状態文字列取得
std::string PlayerBase::GetShootStateString(PLAYER_SHOOT_STATE state) const
{
	switch(state)
	{
		case PLAYER_SHOOT_STATE::NONE:				return "NONE";
		case PLAYER_SHOOT_STATE::SHOOT_READY:		return "SHOOT_READY";		// 発射構え
		case PLAYER_SHOOT_STATE::RIGHT_ARM_SHOOT:	return "RIGHT_ARM_SHOOT";	// 右腕発射
		case PLAYER_SHOOT_STATE::LEFT_ARM_SHOOT:	return "LEFT_ARM_SHOOT";	// 左腕発射
		case PLAYER_SHOOT_STATE::SHOOT_MOVE:		return "SHOOT_MOVE";		// 発射移動
		default:									return "UNKNOWN";
	}
}

// 吸収攻撃状態文字列取得
std::string PlayerBase::GetAbsorbStateString(PLAYER_ABSORB_STATE state) const
{
	switch(state)
	{
		case PLAYER_ABSORB_STATE::NONE:				return "NONE";
		case PLAYER_ABSORB_STATE::ABSORB_READY:		return "ABSORB_READY";	// 吸収構え
		case PLAYER_ABSORB_STATE::ABSORB_ACTIVE:	return "ABSORB_ACTIVE";	// 吸収中
		case PLAYER_ABSORB_STATE::ABSORB_END:		return "ABSORB_END";	// 吸収終了
		default:									return "UNKNOWN";
	}
}

// 特殊状態文字列取得
std::string PlayerBase::GetCombatStateString(PLAYER_COMBAT_STATE state) const
{
	switch(state)
	{
		case PLAYER_COMBAT_STATE::NONE:			return "NONE";
		case PLAYER_COMBAT_STATE::TRANSFORM:	return "TRANSFORM";		// 変身
		case PLAYER_COMBAT_STATE::TRANS_CANCEL:	return "TRSANS_CANCEL";	// 変身解除
		case PLAYER_COMBAT_STATE::GUARD:		return "GUARD";			// ガード
		case PLAYER_COMBAT_STATE::HIT:			return "HIT";			// 被弾
		case PLAYER_COMBAT_STATE::DODGE:		return "DODGE";			// 回避
		case PLAYER_COMBAT_STATE::DEATH:		return "DEATH";			// 死亡
		default:								return "UNKNOWN";
	}
}


