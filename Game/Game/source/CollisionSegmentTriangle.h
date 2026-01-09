#pragma once
#include "appframe.h"

// 線分(カプセル中心線)と三角形の最近接点の計算結果
struct SegmentTriangleResult 
{
	VECTOR vSegMinDistPos = VGet(0.0f, 0.0f, 0.0f);// 線分上の最近接点(カプセル)
	VECTOR vTriMinDistPos = VGet(0.0f, 0.0f, 0.0f);// 三角形上の最近接点
	float fSegTriMinDistSquare = 0.0f;// 最短距離の2乗
};

// min <= x <= max にクランプ
inline float ClapmFloat(float x, float min, float max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

// ベクトルの正規化(ゼロ除算対策)
inline VECTOR SafeNormalizeVec(const VECTOR& v, float eps = 1e-6f)// eps: ゼロとみなす閾値 
{
	const float len = VSize(v);
	if (len < eps) return VGet(0.0f, 1.0f, 0.0f);
	return VScale(v, 1.0f / len);
}

// 点pから線分abへの最近接点を返す
inline VECTOR ClosestPointOnSegment(const VECTOR& vP, const VECTOR& vA, const VECTOR& vB)
{
	const VECTOR vAB = VSub(vB, vA);// 線分のベクトル
	const float abLenSq = VDot(vAB, vAB);
	if (abLenSq <= 1e-8f) return vA;// 線分の長さがほぼゼロならaを返す

	// 射影の計算
	// aからbに向かう方向に、pはどのくらい進んでいるか(距離)
	// 距離を線分の長さで割って正規化し、0.0~1.0にクランプ
	const float t = ClapmFloat(VDot(VSub(vP, vA), vAB) / abLenSq, 0.0f, 1.0f);
	return VAdd(vA, VScale(vAB, t));// 線形補間で実際の座標を求める
}

// 点pから三角形abcへの最近接点を返す
inline VECTOR ClosestPointOnTriangle(const VECTOR& vP, const VECTOR& vA, const VECTOR& vB, const VECTOR& vC)
{
	const VECTOR vAB = VSub(vB, vA);// 三角形の辺ベクトル
	const VECTOR vAC = VSub(vC, vA);
	const VECTOR vAP = VSub(vP, vA);// 頂点aから点pへのベクトル

	// 点pが三角形に対してどこに位置しているかチェック
	// 頂点aの領域(
	const float dot1 = VDot(vAB, vAP);// abとapの内積
	const float dot2 = VDot(vAC, vAP);
	// dot1,dot2が負ならpはaに最も近い
	if (dot1 <= 0.0f && dot2 <= 0.0f) return vA;// 頂点A

	// 頂点bの領域
	const VECTOR vBP = VSub(vP, vB);// 頂点bから点pへのベクトル
	const float dot3 = VDot(vAB, vBP);
	const float dot4 = VDot(vAC, vBP);
	// dot3が正でdot4がdot3以下ならpはbに最も近い(ボロノイ領域の性質)
	if (dot3 >= 0.0f && dot4 <= dot3) return vB;

	// 辺abの領域
	// 三角形abpの符号付き面積
	const float areaC = dot1 * dot4 - dot3 * dot2;
	// 点pが辺abの側面にあり、かつaとbの間にある場合
	if (areaC <= 0.0f && dot1 >= 0.0f && 0.0f <= 0.0f) {
		const float t = dot1 / (dot1 - dot3);// aからどれくらいbよりか(補間係数)
		return VAdd(vA, VScale(vAB, t));// 点pを辺ab上に投影した点
	}

	// 頂点cの領域
	const VECTOR vCP = VSub(vP, vC);// 頂点cから点pへのベクトル
	const float dot5 = VDot(vAB, vCP);
	const float dot6 = VDot(vAC, vCP);
	// 点pが頂点cの外側にある場合
	if (dot6 >= 0.0f && dot5 <= dot6) return vC;// 頂点C

	// 辺acの領域
	// 三角形acpの符号付き面積
	const float areaB = dot5 * dot2 - dot1 * dot6;
	// 点pが辺acの側面にあり、かつaとcの間にある場合
	if (areaB <= 0.0f && dot2 >= 0.0f && dot6 <= 0.0f) {
		const float t = dot2 / (dot2 - dot6);// aからどれくらいcよりか(補間係数)
		return VAdd(vA, VScale(vAC, t));// 点pを辺ac上に投影した点
	}

	// 辺bcの領域
	// 三角形bcpの符号付き面積
	const float areaA = dot3 * dot6 - dot5 * dot4;
	// 点pが辺bcの側面にあり、かつbとcの間にある場合
	if (areaA <= 0.0f && (dot4 - dot3) >= 0.0f && (dot5 - dot6) >= 0.0f) {
		const VECTOR vBC = VSub(vC, vB);// 辺bcのベクトル
		const float t = (dot4 - dot3) / ((dot4 - dot3) + (dot5 - dot6));// bからどれくらいcよりか(補間係数)
		return VAdd(vB, VScale(vBC, t));// 点pを辺bc上に投影した点
	}

	// どの領域にも該当しない場合、三角形の面内に存在する
	// 重心座標で射影
	const float denom = 1.0f / (areaA + areaB + areaC);// 正規化のための係数
	const float weightB = areaB * denom;// 頂点bの重み
	const float weightC = areaC * denom;// 頂点cの重み
	return VAdd(vA, VAdd(VScale(vAB, weightB), VScale(vAC, weightC)));// 面上のどの位置にいるか
}

// 線分pq上の、比率t(0.0~1.0)の点を返す(線形補間)
inline VECTOR PointOnSegment(const VECTOR& vP, const VECTOR& vQ, float t)
{
	return VAdd(vP, VScale(VSub(vQ, vP), t));
}

// 線分と三角形の最短距離(最近接点ペア)を返す
// 1.線分と三角形が交差するなら距離0(最近接点は交点)
// 2.交差しないなら候補から最短距離を求める
inline SegmentTriangleResult SegmentTriangleMinDistance(const VECTOR& vSegP, const VECTOR& vSegQ,
	const VECTOR& vA, const VECTOR& vB, const VECTOR& vC) 
{
	SegmentTriangleResult out{};

	// 1.交差判定(線分 vs 三角形平面 → 交点が三角形内なら距離0)
	const VECTOR vAB = VSub(vB, vA);
	const VECTOR vAC = VSub(vC, vA);
	const VECTOR vN = SafeNormalizeVec(VCross(vAB, vAC));// 三角形の法線ベクトル

	const float planeDot = VDot(vN, vA);// 平面の方程式の内積を求める
	// 線分の両端点が平面のどちら側にあるか
	const float distP = VDot(vN, vSegP) - planeDot;
	const float distQ = VDot(vN, vSegQ) - planeDot;

	// 平面を跨ぐ可能性
	if (distP * distQ <= 0.0f) {// 符号が異なる(または0)なら跨いでいる
		const float denom = distP - distQ;
		if (std::fabs(denom) > 1e-8f) {// ゼロ除算防止
			// 平面上の交点座標xを求める
			const float t = ClapmFloat(distP / (distP - distQ), 0.0f, 1.0f);
			const VECTOR vX = PointOnSegment(vSegP, vSegQ, t);

			// 交点xが三角形内にあるか判定(同側判定)
			const VECTOR vBC = VSub(vC, vB);// 三角形の3つの辺を求める
			const VECTOR vCA = VSub(vA, vC);

			// 外積による向きの計算
			const VECTOR vC0 = VCross(vAB, VSub(vX, vA));// 辺abとベクトルaxの外積
			const VECTOR vC1 = VCross(vBC, VSub(vX, vB));// 辺bcとベクトルbxの外積
			const VECTOR vC2 = VCross(vCA, VSub(vX, vC));// 辺caとベクトルcxの外積

			// 3つの外積すべてが、三角形の法線と同じ方向を向いているか
			if (VDot(vC0, vN) >= 0.0f && VDot(vC1, vN) >= 0.0f && VDot(vC2, vN) >= 0.0f) {// 内積が正なら角度が90度未満
				// 三角形の中にある
				out.vSegMinDistPos = vX;
				out.vTriMinDistPos = vX;
				out.fSegTriMinDistSquare = 0.0f;// 距離0
				return out;
			}
		}
	}

	// 2.交差しない場合、候補から最少を選ぶ
	// ①線分始点pから三角形への最近点
	VECTOR vTriP = ClosestPointOnTriangle(vSegP, vA, vB, vC);
	float bestDistSq = VDot(VSub(vSegP, vTriP), VSub(vSegP, vTriP));// 距離の2乗
	// これを暫定最小値として初期化
	VECTOR vBestSeg = vSegP;
	VECTOR vBestTri = vTriP;

	// ②線分終点qから三角形への最近点
	VECTOR vTriQ = ClosestPointOnTriangle(vSegQ, vA, vB, vC);
	float distSqQ = VDot(VSub(vSegQ, vTriQ), VSub(vSegQ, vTriQ));
	// 最小値より近いか
	if (distSqQ < bestDistSq) {
		// 最小値を更新
		bestDistSq = distSqQ;
		vBestSeg = vSegQ;
		vBestTri = vTriQ;
	}

	// ③三角形頂点aから線分への最近点
	VECTOR vSegA = ClosestPointOnSegment(vA, vSegP, vSegQ);
	float distSqA = VDot(VSub(vA, vSegA), VSub(vA, vSegA));
	if (distSqA < bestDistSq) {
		bestDistSq = distSqA;
		vBestSeg = vSegA;
		vBestTri = vA;// 頂点a
	}

	// ④三角形頂点bから線分への最近点
	VECTOR vSegB = ClosestPointOnSegment(vB, vSegP, vSegQ);
	float distSqB = VDot(VSub(vB, vSegB), VSub(vB, vSegB));
	if (distSqB < bestDistSq) {
		bestDistSq = distSqB;
		vBestSeg = vSegB;
		vBestTri = vB;// 頂点b
	}

	// ⑤三角形頂点cから線分への最近点
	VECTOR vSegC = ClosestPointOnSegment(vC, vSegP, vSegQ);
	float distSqC = VDot(VSub(vC, vSegC), VSub(vC, vSegC));
	if (distSqC < bestDistSq) {
		bestDistSq = distSqC;
		vBestSeg = vSegC;
		vBestTri = vC;// 頂点c
	}

	// 線分と三角形の間の最も近い2点
	out.vSegMinDistPos = vBestSeg;
	out.vTriMinDistPos = vBestTri;
	out.fSegTriMinDistSquare = bestDistSq;
	return out;

}
