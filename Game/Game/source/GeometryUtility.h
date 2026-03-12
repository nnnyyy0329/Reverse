#pragma once
#include "appframe.h"

// 扇形の基本データ構造体
struct SectorData
{
	VECTOR center;				// 中心点
	VECTOR direction;			// 方向ベクトル  
	float range;				// 範囲
	float angle;				// 角度（ラジアン）
	float heightOffset;			// 高さオフセット
};

namespace GeometryUtility
{
	// 対象が前方にあるかどうかを判定する
	bool IsFacing(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos, float dotThreshold = 0.0f);
		
	// 攻撃が前方からかどうかを判定する
	bool IsAttackFromFront(const VECTOR& targetPos, const VECTOR& targetDir, const VECTOR& attackDir, float dotThreshold = 0.0f);

	// 対象の内積の値を取得
	float GetDotToTarget(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos);

	// 攻撃方向からの内積の値を取得
	float GetDotFromAttack(const VECTOR& targetDir, const VECTOR& attackDir);

	// 2つのベクトル間の角度を取得（ラジアン）
	float GetAngleRad(const VECTOR& vec1, const VECTOR& vec2);

	// 2つのベクトル間の角度を取得（度数法）
	float GetAngleDeg(const VECTOR& vec1, const VECTOR& vec2);

	// 扇形内にいるかチェック
	bool IsInSector(const VECTOR& targetPos, const SectorData& sectorData);

	// 扇形の描画用の点を計算
	void DrawSector(const SectorData& sectorData, int division, int fillColor, int lineColor);
}

