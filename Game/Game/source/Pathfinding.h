#pragma once
#include "appframe.h"

namespace Pathfinding
{
	struct WayPoint
	{
		int id;
		VECTOR vPos;
		std::vector<int> links;// つながっているウェイポイントのIDリスト
	};

	// 2点間の距離を計算
	float GetDistance(VECTOR p1, VECTOR p2);

	int GetNearestWayPoint(VECTOR pos, const std::vector<WayPoint>& wps);
}

