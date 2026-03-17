#pragma once	// .hの先頭に記述。#includeでこのファイルを何度読み込みしても、1度しか読み込まない
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



// 汎用数学・計算ユーティリティ
#undef max
#undef min
namespace mymath
{
	// 線形補間 : tを(0,1)にクランプしてstartとendの間を補間する
	inline float Lerp(float start, float end, float t)
	{
		t = std::max(0.0f, std::min(1.0f, t));
		return start + (end - start) * t;
	}

	// ベクトル線形補間 : tを(0,1)にクランプしてstartとendの間を補間する
	inline VECTOR VectorLerp(const VECTOR& start, const VECTOR& end, float t)
	{
		t = std::max(0.0f, std::min(1.0f, t));
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

	// DrawLine3Dを組み合わせて3D空間に円を描画する関数
	// vCenter:中心座標, fRadius:半径, color:色, segment:分割数
	inline void DrawCircle3D(const VECTOR& vCenter, float fRadius, unsigned int color, int segments)
	{
		float step = DX_TWO_PI_F / segments;

		VECTOR vPrev = VGet(
			vCenter.x + sinf(0.0f) * fRadius,
			vCenter.y,
			vCenter.z + cosf(0.0f) * fRadius
		);

		for (int i = 1; i <= segments; ++i)
		{
			float angle = step * i;
			VECTOR vNext = VGet(
				vCenter.x + sinf(angle) * fRadius,
				vCenter.y,
				vCenter.z + cosf(angle) * fRadius
			);
			DrawLine3D(vPrev, vNext, color);
			vPrev = vNext;
		}
	}


	// DrawLine3Dを組み合わせて3D空間に扇形を描画する関数
	// vCenter:中心座標, vDir:基準の向きベクトル, fRadius:半径, fHalfAngleDeg:半角(度), color:色, segments:分割数
	inline void DrawFan3D(const VECTOR& vCenter, const VECTOR& vDir, float fRadius, float fHalfAngleDeg, unsigned int color, int segments)
	{
		float halfAngleRad = fHalfAngleDeg * DEGREE_TO_RADIAN;
		float currentDirAngle = atan2f(vDir.x, vDir.z);
		float startAngle = currentDirAngle - halfAngleRad;
		float totalAngle = halfAngleRad * 2.0f;

		VECTOR vLeftEdge = VAdd(vCenter, VGet(sinf(startAngle) * fRadius, 0.0f, cosf(startAngle) * fRadius));
		float  endAngle = startAngle + totalAngle;
		VECTOR vRightEdge = VAdd(vCenter, VGet(sinf(endAngle) * fRadius, 0.0f, cosf(endAngle) * fRadius));

		DrawLine3D(vCenter, vLeftEdge, color);
		DrawLine3D(vCenter, vRightEdge, color);

		VECTOR vPrev = vLeftEdge;
		for (int i = 1; i <= segments; ++i)
		{
			float ratio = static_cast<float>(i) / static_cast<float>(segments);
			float angle = startAngle + totalAngle * ratio;
			VECTOR vNext = VAdd(vCenter, VGet(sinf(angle) * fRadius, 0.0f, cosf(angle) * fRadius));
			DrawLine3D(vPrev, vNext, color);
			vPrev = vNext;
		}
	}
}
