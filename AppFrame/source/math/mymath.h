#pragma once	// .hの先頭に記述。#includeでこのファイルを何度読み込みしても、1度しか読み込まない
#include <math.h>
#include "DxLib.h"

// 円周率
#define	PI				(3.141592653589793)

// degreeとradianの変換
static constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
static constexpr auto RADIAN_TO_DEGREE = 180.0f / DX_PI_F;

class Character;

// 当たり判定用。2つのboxが当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBox
(
	int x1, int y1, int w1, int h1,		// ひとつめのbox 左上(x,y), 大きさw,h
	int x2, int y2, int w2, int h2		// ふたつめのbox 左上(x,y), 大きさw,h
);


// 当たり判定用。2つの円が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitCircle
(
	int x1, int y1, int r1,		// ひとつめのcircle 中心(x,y), 半径r
	int x2, int y2, int r2		// ふたつめのcircle 中心(x,y), 半径r
);

// 当たり判定用。円と矩形が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBoxCircle
(
	int cx, int cy, int r,			// circle 中心(cx,cy), 半径r
	int x, int y, int w, int h	//  左上(x,y), 大きさw,h
);

// カプセルと点の最短距離の二乗を計算
float GetCapsulePointSq(const VECTOR& point, const VECTOR& capsuleTop, const VECTOR& capsuleBottom, float capsuleRadius);

// カプセルと球の当たり判定
bool HitCheck_Capsule_Sphere(const VECTOR& capsuleTop, const VECTOR& capsuleBottom, float capsuleRadius, const VECTOR& sphereCenter, float sphereRadius);

// 点と線分の最短距離の二乗を計算
float GetPointSegmentSq(const VECTOR& point, const VECTOR& segmentStart, const VECTOR& segmentEnd);

// 円形床判定用の構造体
struct CircleFloor
{
	VECTOR center; // 円の中心位置
	float radius;  // 円の半径
	float height;  // 円の高さ

	// 点が円内にあるか
	bool IsPointInside(const VECTOR& point) const;

	// 円の境界までの距離を計算
	float GetDistEdge(const VECTOR& point) const;
};
