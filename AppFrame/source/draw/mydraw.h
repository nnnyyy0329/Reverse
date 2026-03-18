#pragma once
#include "DxLib.h"
#include "../math/mymath.h"

// 描画ユーティリティ
namespace mydraw
{
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
