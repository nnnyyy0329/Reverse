#pragma once
#include "appframe.h"

// 線分(カプセル中心線)と三角形の最近接点解析結果
struct SegmentTriangleResult 
{
	VECTOR vSegMinDistPos = VGet(0.0f, 0.0f, 0.0f);// 線分上の最近接点
	VECTOR vTriMinDistPos = VGet(0.0f, 0.0f, 0.0f);// 三角形上の最近接点
	float fSegTriMinDistSquare = 0.0f;// 最短距離^2
};

//inline float ClampFloat(float x, float minv, float maxv)
//{
//	return std::max(minv, std::min(maxv, x));
//}

// min <= x <= max にクランプ
inline float ClapmFloat(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline VECTOR SafeNormalizeVec(const VECTOR& v, float eps = 1e-6f) 
{
	const float len = VSize(v);
	if (len < eps) return VGet(0.0f, 1.0f, 0.0f);
	return VScale(v, 1.0f / len);
}

// 点pから線分abへの最近接点を返す
inline VECTOR ClosestPointOnSegment(const VECTOR& vP, const VECTOR& vA, const VECTOR& vB)
{
	const VECTOR vAB = VSub(vB, vA);
	const float abLenSq = VDot(vAB, vAB);
	if (abLenSq <= 1e-8f) return vA;

	const float t = ClapmFloat(VDot(VSub(vP, vA), vAB) / abLenSq, 0.0f, 1.0f);
	return VAdd(vA, VScale(vAB, t));
}

//// 点pから三角形abcへの最近接点を返す（Ericson本の定番実装）
//inline VECTOR ClosestPointOnTriangle(const VECTOR& p, const VECTOR& a, const VECTOR& b, const VECTOR& c)
//{
//	const VECTOR ab = VSub(b, a);
//	const VECTOR ac = VSub(c, a);
//	const VECTOR ap = VSub(p, a);
//
//	const float d1 = VDot(ab, ap);
//	const float d2 = VDot(ac, ap);
//	if (d1 <= 0.0f && d2 <= 0.0f) return a; // 頂点A
//
//	const VECTOR bp = VSub(p, b);
//	const float d3 = VDot(ab, bp);
//	const float d4 = VDot(ac, bp);
//	if (d3 >= 0.0f && d4 <= d3) return b; // 頂点B
//
//	const float vc = d1 * d4 - d3 * d2;
//	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
//		const float v = d1 / (d1 - d3);
//		return VAdd(a, VScale(ab, v)); // 辺AB
//	}
//
//	const VECTOR cp = VSub(p, c);
//	const float d5 = VDot(ab, cp);
//	const float d6 = VDot(ac, cp);
//	if (d6 >= 0.0f && d5 <= d6) return c; // 頂点C
//
//	const float vb = d5 * d2 - d1 * d6;
//	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
//		const float w = d2 / (d2 - d6);
//		return VAdd(a, VScale(ac, w)); // 辺AC
//	}
//
//	const float va = d3 * d6 - d5 * d4;
//	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
//		const VECTOR bc = VSub(c, b);
//		const float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
//		return VAdd(b, VScale(bc, w)); // 辺BC
//	}
//
//	// 面内：重心座標で射影
//	const float denom = 1.0f / (va + vb + vc);
//	const float v = vb * denom;
//	const float w = vc * denom;
//	return VAdd(a, VAdd(VScale(ab, v), VScale(ac, w)));
//}

// 点pから三角形abcへの最近接点を返す
inline VECTOR ClosestPointOnTriangle(const VECTOR& vP, const VECTOR& vA, const VECTOR& vB, const VECTOR& vC)
{
	const VECTOR vAB = VSub(vB, vA);
	const VECTOR vAC = VSub(vC, vA);
	const VECTOR vAP = VSub(vP, vA);

	const float d1 = VDot(vAB, vAP);
	const float d2 = VDot(vAC, vAP);
	if (d1 <= 0.0f && d2 <= 0.0f) return vA;// 頂点A

	const VECTOR vBP = VSub(vP, vB);
	const float d3 = VDot(vAB, vBP);
	const float d4 = VDot(vAC, vBP);
	if (d3 >= 0.0f && d4 <= d3) return vB;// 頂点B
}

// 線分pq上のt(0..1)の点
inline VECTOR PointOnSegment(const VECTOR& p, const VECTOR& q, float t)
{
	return VAdd(p, VScale(VSub(q, p), t));
}

// 線分と三角形の最短距離（最近接点ペア）を返す
// 方針：
// 1) 線分が三角形と交差するなら距離0（最近接点は交点）
// 2) 交差しないなら候補列挙で最小を取る
inline SegmentTriangleResult SegmentTriangleMinDistance(const VECTOR& segP, const VECTOR& segQ,
	const VECTOR& a, const VECTOR& b, const VECTOR& c)
{
	SegmentTriangleResult out{};

	// 1) 交差判定（線分 vs 三角形平面 → 交点が三角形内なら距離0）
	const VECTOR ab = VSub(b, a);
	const VECTOR ac = VSub(c, a);
	const VECTOR n = SafeNormalizeVec(VCross(ab, ac));

	const float planeD = VDot(n, a);
	const float distP = VDot(n, segP) - planeD;
	const float distQ = VDot(n, segQ) - planeD;

	// 平面を跨ぐ可能性
	if (distP * distQ <= 0.0f) {
		const float denom = distP - distQ;
		if (std::fabs(denom) > 1e-8f) {
			const float t = ClampFloat(distP / (distP - distQ), 0.0f, 1.0f);
			const VECTOR x = PointOnSegment(segP, segQ, t);

			// 三角形内判定（同側判定）
			const VECTOR bc = VSub(c, b);
			const VECTOR ca = VSub(a, c);

			const VECTOR c0 = VCross(ab, VSub(x, a));
			const VECTOR c1 = VCross(bc, VSub(x, b));
			const VECTOR c2 = VCross(ca, VSub(x, c));

			if (VDot(c0, n) >= 0.0f && VDot(c1, n) >= 0.0f && VDot(c2, n) >= 0.0f) {
				out.Seg_MinDist_Pos = x;
				out.Tri_MinDist_Pos = x;
				out.Seg_Tri_MinDist_Square = 0.0f;
				return out;
			}
		}
	}

	// 2) 交差しない場合：候補列挙で最小を取る
	VECTOR triP = ClosestPointOnTriangle(segP, a, b, c);
	float bestSq = VDot(VSub(segP, triP), VSub(segP, triP));
	VECTOR bestSeg = segP;
	VECTOR bestTri = triP;

	VECTOR triQ = ClosestPointOnTriangle(segQ, a, b, c);
	float sqQ = VDot(VSub(segQ, triQ), VSub(segQ, triQ));
	if (sqQ < bestSq) {
		bestSq = sqQ;
		bestSeg = segQ;
		bestTri = triQ;
	}

	VECTOR segA = ClosestPointOnSegment(a, segP, segQ);
	float sqA = VDot(VSub(a, segA), VSub(a, segA));
	if (sqA < bestSq) {
		bestSq = sqA;
		bestSeg = segA;
		bestTri = a;
	}

	VECTOR segB = ClosestPointOnSegment(b, segP, segQ);
	float sqB = VDot(VSub(b, segB), VSub(b, segB));
	if (sqB < bestSq) {
		bestSq = sqB;
		bestSeg = segB;
		bestTri = b;
	}

	VECTOR segC = ClosestPointOnSegment(c, segP, segQ);
	float sqC = VDot(VSub(c, segC), VSub(c, segC));
	if (sqC < bestSq) {
		bestSq = sqC;
		bestSeg = segC;
		bestTri = c;
	}

	out.Seg_MinDist_Pos = bestSeg;
	out.Tri_MinDist_Pos = bestTri;
	out.Seg_Tri_MinDist_Square = bestSq;
	return out;
}
