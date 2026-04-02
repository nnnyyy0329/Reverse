/*****************************************
* file   GeometryUtility.h
* brief  幾何計算用の便利関数クラス
* author 成田 悠真
* date   2026/02/06
******************************************/

#pragma once
#include "appframe.h"

/// @brief 扇形の基本データ構造体
struct SectorData
{
	VECTOR center;		// 中心点
	VECTOR direction;	// 方向ベクトル  
	float range;		// 範囲
	float angle;		// 角度
	float heightOffset;	// 高さオフセット
};

/// @brief 幾何計算用の便利関数クラス
namespace GeometryUtility
{
	/// @brief 対象が前方にあるかどうかを判定する関数
	///
	/// @param fromPos 判定元の位置
	/// @param fromDir 判定元の方向ベクトル
	/// @param targetPos 判定対象の位置
	/// @param dotThreshold 前方判定の内積閾値(デフォルトは0.0f)
	/// 
	/// @return 対象が前方にある場合はtrue、そうでない場合はfalse
	bool IsFacing(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos, float dotThreshold = 0.0f);
	
	/// @brief 攻撃が前方からかどうかを判定する関数
	///
	/// @param targetPos 対象の位置
	/// @param targetDir 対象の方向ベクトル
	/// @param attackDir 攻撃の方向ベクトル
	/// @param dotThreshold 前方判定の内積閾値(デフォルトは0.0f)
	/// 
	/// @return 攻撃が前方からの場合はtrue、そうでない場合はfalse
	bool IsAttackFromFront(const VECTOR& targetPos, const VECTOR& targetDir, const VECTOR& attackDir, float dotThreshold = 0.0f);

	/// @brief 対象の内積の値を取得する関数
	///
	/// @param fromPos 判定元の位置
	/// @param fromDir 判定元の方向ベクトル
	/// @param targetPos 判定対象の位置
	/// 
	/// @return 対象の内積の値
	float GetDotToTarget(const VECTOR& fromPos, const VECTOR& fromDir, const VECTOR& targetPos);

	/// @brief 攻撃方向からの内積の値を取得する関数
	///
	/// @param targetDir 対象の方向ベクトル
	/// @param attackDir 攻撃の方向ベクトル
	/// 
	///	@return 攻撃方向からの内積の値
	float GetDotFromAttack(const VECTOR& targetDir, const VECTOR& attackDir);

	// 2つのベクトル間の角度を取得（ラジアン）

	/// @brief 2つのベクトル間の角度を取得する関数（ラジアン）
	///
	/// @param vec1 1つ目のベクトル
	/// @param vec2 2つ目のベクトル
	/// 
	/// @return 2つのベクトル間の角度（ラジアン）
	float GetAngleRad(const VECTOR& vec1, const VECTOR& vec2);

	/// @brief 2つのベクトル間の角度を取得する関数（度数法）
	///
	/// @param vec1 1つ目のベクトル
	/// @param vec2 2つ目のベクトル
	/// 
	/// @return 2つのベクトル間の角度（度数法）
	float GetAngleDeg(const VECTOR& vec1, const VECTOR& vec2);


	/* 扇形 */

	/// @brief 対象が扇形内にいるかチェックする関数
	///
	/// @param targetPos 対象の位置
	/// @param sectorData 扇形の基本データ
	/// 
	/// @return 対象が扇形内にいる場合はtrue、そうでない場合はfalse
	bool IsInSector(const VECTOR& targetPos, const SectorData& sectorData);

	// 扇形の描画用の点を計算

	/// @brief 扇形の描画用の点を計算する関数
	///
	/// @param sectorData 扇形の基本データ
	/// @param division 扇形の分割数
	/// @param fillColor 塗りつぶし色
	/// @param lineColor 線の色
	void DrawSector(const SectorData& sectorData, int division, int fillColor, int lineColor);


	/* 変換 */

	/// @brief ローカル座標のオフセットをワールド座標に変換する関数
	///
	/// @param offset ローカル座標のオフセット
	/// @param playerDir プレイヤーの向きベクトル
	/// 
	/// @return ワールド座標に変換されたオフセット
	VECTOR TransOffsetToWorld(const VECTOR& offset, const VECTOR& playerDir);						
}

