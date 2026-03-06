#pragma once
#include "appframe.h"

class StageBase;

namespace Pathfinding
{
	// ウェイポイント
	struct Waypoint 
	{
		int id;
		VECTOR vPos;
		std::vector<int> links;// つながっているウェイポイントのID
	};

	// 経路探索管理クラス
	class Manager
	{
	public:
		Manager() = default;
		virtual ~Manager() = default;

		// ウェイポイントの追加
		void AddWaypoint(int id, VECTOR pos);

		// ウェイポイント同士のつながりを構築
		void BuildWaypointLinks(float capRad, const StageBase* stage);

		// A*アルゴリズムでスタートからゴールまでの最短ルートの座標リストを計算
		std::vector<VECTOR> FindPath(int startId, int goalId);

		// 指定した座標から一番近いウェイポイントのIDを取得
		int GetNearestWaypoint(VECTOR pos, float capRad, const StageBase* stage);

		const std::vector<Waypoint>& GetWaypoints() { return _waypoints; }

		bool CheckCapsuleLineObstacle(VECTOR start, VECTOR end, float capRad, const StageBase* stage);

	private:
		float GetDistance(VECTOR p1, VECTOR p2);

		std::vector<Waypoint> _waypoints;
	};
}

