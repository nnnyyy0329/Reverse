#pragma once// .hの先頭に記述。#includeでこのファイルを何度読み込みしても、1度しか読み込まない
#include <math.h>
#include <random>
#include <algorithm>
#include "DxLib.h"

// 計算用マクロ
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

// degreeとradianの変換
static constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
static constexpr auto RADIAN_TO_DEGREE = 180.0f / DX_PI_F;

// 当たり判定用。2つのboxが当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBox
(
	int x1, int y1, int w1, int h1,// ひとつめのbox 左上(x,y), 大きさw,h
	int x2, int y2, int w2, int h2// ふたつめのbox 左上(x,y), 大きさw,h
);


// 当たり判定用。2つの円が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitCircle
(
	int x1, int y1, int r1,// ひとつめのcircle 中心(x,y), 半径r
	int x2, int y2, int r2// ふたつめのcircle 中心(x,y), 半径r
);

// 当たり判定用。円と矩形が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBoxCircle
(
	int cx, int cy, int r,// circle 中心(cx,cy), 半径r
	int x, int y, int w, int h//  左上(x,y), 大きさw,h
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
	VECTOR center;// 円の中心位置
	float radius;// 円の半径
	float height;// 円の高さ

	// 点が円内にあるか
	bool IsPointInside(const VECTOR& point) const;

	// 円の境界までの距離を計算
	float GetDistEdge(const VECTOR& point) const;
};





// 汎用数学・計算ユーティリティ
#undef max
#undef min
namespace mymath
{
	// 値をminValueとmaxValueの範囲に収める
	template<typename T>
	inline T Clamp(const T& value, const T& minValue, const T& maxValue)
	{
		return std::max(minValue, std::min(maxValue, value));
	}

	// 0.0fから1.0fの範囲に値を収める
	inline float Clamp01(float value)
	{
		return Clamp(value, 0.0f, 1.0f);
	}

	// 線形補間 : tを(0,1)にクランプしてstartとendの間を補間する
	inline float Lerp(float start, float end, float t)
	{
		t = Clamp01(t);
		return start + (end - start) * t;
	}

	// ベクトル線形補間 : tを(0,1)にクランプしてstartとendの間を補間する
	inline VECTOR VectorLerp(const VECTOR& start, const VECTOR& end, float t)
	{
		t = Clamp01(t);
		return VAdd(start, VScale(VSub(end, start), t));
	}

	// 角度正規化 : 角度(ラジアン)を(-PI, +PI)の範囲に収める
	inline float WrapAngle(float angle)
	{
		while (angle > DX_PI_F)
		{
			angle -= 2.0f * DX_PI_F;
		}
		while (angle < -DX_PI_F)
		{
			angle += 2.0f * DX_PI_F;
		}
		return angle;
	}

	// 乱数生成 : (min, max)の範囲でランダムなfloatを返す
	inline float RandomRange(float min, float max)
	{
		static std::mt19937 engine(std::random_device{}());
		std::uniform_real_distribution<float> dist(min, max);
		return dist(engine);
	}

	// Y成分を除去して水平方向の単位ベクトルを返す(長さが極小の場合はゼロベクトル)
	inline VECTOR FlattenVector(const VECTOR& v)
	{
		VECTOR flat = v;
		flat.y = 0.0f;
		if (VSquareSize(flat) > 0.0001f)
		{
			return VNorm(flat);
		}
		return VGet(0.0f, 0.0f, 0.0f);
	}
}
