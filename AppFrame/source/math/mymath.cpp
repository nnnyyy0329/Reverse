// 独自計算ライブラリ
#include "mymath.h"

// 当たり判定用。2つのboxが当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBox(
	int x1, int y1, int w1, int h1,		// ひとつめのbox 左上(x,y), 大きさw,h
	int x2, int y2, int w2, int h2		// ふたつめのbox 左上(x,y), 大きさw,h
	)
{
	if (x1 < x2 + w2 && x2 < x1 + w1		// x方向の判定
		&& y1 < y2 + h2 && y2 < y1 + h1		// y方向の判定
		)
	{
		// 2つのboxは当たっている
		return 1;
	}

	// 2つのboxは当たっていない
	return 0;
}

// 当たり判定用。2つの円が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitCircle(
	int x1, int y1, int r1,		// ひとつめのcircle 中心(x,y), 半径r
	int x2, int y2, int r2		// ふたつめのcircle 中心(x,y), 半径r
	)
{
	int w, h, r;
	w = x1 - x2;
	h = y1 - y2;
	r = r1 + r2;
	if (r * r > w * w + h * h) 
	{
		return 1;
	}
	return 0;
}

// 当たり判定用。円と矩形が当たったかを判定
// 当たっていたら1, 当たっていなかったら0を返す
int IsHitBoxCircle(
	int cx, int cy, int r,			// circle 中心(cx,cy), 半径r
	int x, int y, int w, int h	// 左上(x,y), 大きさw,h
	)
{
	// 円の中心から矩形までの最短距離を計算
	int close_x = cx;
	int close_y = cy;

	// 円の中心のx座標が矩形の範囲内にあるかチェック
	if (cx < x) 
	{
		close_x = x;
	}
	else if (cx > x + w)
	{
		close_x = x + w;
	}

	// 円の中心のy座標が矩形の範囲内にあるかチェック
	if (cy < y) 
	{
		close_y = y;
	}
	else if (cy > y + h)
	{
		close_y = y + h;
	}

	// 円の中心と最短点との距離を計算
	int dx = cx - close_x;
	int dy = cy - close_y;

	// 距離が半径以下なら当たっている
	if (dx * dx + dy * dy <= r * r)
	{
		return 1;
	}
	return 0;
}

// 点と線分の最短距離の二乗を計算
float GetPointSegmentSq(const VECTOR& point, const VECTOR& segmentStart, const VECTOR& segmentEnd)
{
	// 線分のベクトル
	VECTOR segmentVec = VSub(segmentEnd, segmentStart);

	// 線分の開始点から点へのベクトル
	VECTOR toPoint = VSub(point, segmentStart);

	// 線分の長さの二乗
	float segmentLenSq = VDot(segmentVec, segmentVec);

	// 線分の長さが0の場合(点と点の距離)
	if (segmentLenSq < 1e-6f)
	{
		return VDot(toPoint, toPoint); // 点と開始点の距離の二乗を返す
	}

	// 線分上の最近点を求める
	float t = VDot(toPoint, segmentVec) / segmentLenSq;

	// tを0から1の範囲にクランプ
	if (t < 0.f) t = 0.f;
	else if (t > 1.f) t = 1.f;

	// 線分上の最近点
	VECTOR closestPoint = VAdd(segmentStart, VScale(segmentVec, t));

	// 点と最近点の距離の二乗を計算
	VECTOR diff = VSub(point, closestPoint);

	// 距離の二乗を返す
	return VDot(diff, diff);
}

// カプセルと点の最短距離の二乗を計算
float GetCapsulePointSq(const VECTOR& point, const VECTOR& capsuleTop, const VECTOR& capsuleBottom, float capsuleRadius)
{
	// カプセルの中心軸と点の最短距離の二乗を計算
	float distSq = GetPointSegmentSq(point, capsuleTop, capsuleBottom);
	return distSq;
}

// カプセルと球の当たり判定
bool HitCheck_Capsule_Sphere(const VECTOR& capsuleTop, const VECTOR& capsuleBottom, float capsuleRadius, const VECTOR& sphereCenter, float sphereRadius)
{
	// カプセルの中心軸と球の中心点の最短距離の二乗を計算
	float distSq = GetCapsulePointSq(sphereCenter, capsuleTop, capsuleBottom, capsuleRadius);

	// 半径の合計の二乗
	float totalRadius = capsuleRadius + sphereRadius;
	float totalRadiusSq = totalRadius * totalRadius;

	// 最短距離の二乗が、半径の合計の二乗以下なら当たり
	if (distSq <= totalRadiusSq)
	{
		return true;
	}

	// 当たっていない場合はfalseを返す
	return false;
}

// 点が円内にあるか
bool CircleFloor::IsPointInside(const VECTOR& point) const
{
	// 円の中心と点の距離の二乗を計算
	float dx = point.x - center.x;
	float dz = point.z - center.z;
	float distSq = dx * dx + dz * dz;

	// 半径の二乗と比較
	float radiusSq = radius * radius;
	return distSq <= radiusSq;
}

// 円の境界までの距離を計算
float CircleFloor::GetDistEdge(const VECTOR& point) const
{
	// 円の中心と点の距離を計算
	float dx = point.x - center.x;
	float dz = point.z - center.z;
	float dist = sqrtf(dx * dx + dz * dz);

	// 境界までの距離を計算
	return  radius - dist;
}