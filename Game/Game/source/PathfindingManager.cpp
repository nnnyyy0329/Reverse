#include "PathfindingManager.h"
#include "StageBase.h"

namespace Pathfinding
{
	// A*アルゴリズムのノード構造体
	struct AStarNode
	{
		int id;
		float gCost;// スタートからこのノードまでの移動距離
		float hCost;// このノードからゴールまでの直線距離
		float fCost() { return gCost + hCost; }// 総コスト(この値が一番小さい道を選ぶ)
		int parentId;// どこから来たかを記録(逆順で辿るため)
	};

	// ステージの初期化時に呼ぶ。指定IDと座標でウェイポイントを追加
	void Manager::AddWaypoint(int id, VECTOR pos)
	{
		Waypoint wp;
		wp.id = id;
		wp.vPos = pos;
		_waypoints.push_back(wp);
	}

	// 2点間の直線距離を計算
	float Manager::GetDistance(VECTOR p1, VECTOR p2)
	{
		return VSize(VSub(p1, p2));
	}

	// 経路探索のスタート、ゴールを決める
	// 全ウェイポイントとの距離を総当たりで計算して、最も近いウェイポイントのIDを返す
	int Manager::GetNearestWaypoint(VECTOR pos, float capRad, const StageBase* stage)
	{
		if (_waypoints.empty()) { return -1; }

		int nearestId = -1;
		float minDist = 999999.0f;

		// 1.間に壁がないウェイポイントの中で一番近いものを探す
		for (const auto& wp : _waypoints)
		{
			float dist = GetDistance(pos, wp.vPos);
			if (dist < minDist)
			{
				// 壁がないかチェック
				if(!CheckCapsuleLineObstacle(pos, wp.vPos, capRad, stage))
				{
					minDist = dist;
					nearestId = wp.id;
				}
			}
		}

		// 2.もし視線が通るポイントがなかった場合、壁を無視して一番近いものを返す
		if (nearestId == -1)
		{
			minDist = 999999.0f;
			for(const auto& wp : _waypoints)
			{
				float dist = GetDistance(pos, wp.vPos);
				if (dist < minDist)
				{
					minDist = dist;
					nearestId = wp.id;
				}
			}
		}

		return nearestId;
	}

	// 2点間に障害物がないかチェック
	// スタートからエンドまでの線分(カプセル)とステージのマップモデルの判定を行う
	bool Manager::CheckCapsuleLineObstacle(VECTOR start, VECTOR end, float capRad, const StageBase* stage)
	{
		if (!stage) { return true; }

		VECTOR checkStart = start;
		VECTOR checkEnd = end;
		float yOffset = 50.0f;// 足元から少し上げる
		checkStart.y += yOffset;
		checkEnd.y += yOffset;
		float checkRad = capRad * 0.8f;// 少し小さめで判定する

		const auto& mapObjList = stage->GetMapModelPosList();

		for (const auto& obj : mapObjList)
		{
			if (obj.collisionFrame == -1 || obj.modelHandle <= 0) { continue; }

			MV1_COLL_RESULT_POLY_DIM result = MV1CollCheck_Capsule(
				obj.modelHandle,
				obj.collisionFrame,
				checkStart,
				checkEnd,
				checkRad
			);

			bool hitWall = false;

			if (result.HitNum > 0)
			{
				for (int i = 0; i < result.HitNum; ++i)
				{
					if (result.Dim[i].Normal.y < 0.3f)
					{
						hitWall = true;
						break;// 1つでも当たったらループを抜ける
					}
				}
			}

			MV1CollResultPolyDimTerminate(result);

			if (hitWall) { return true; }// 障害物ありとして終了
		}

		return false;// 障害物なし
	}

	// ステージ読み込み後に一度だけ実行
	// 全ウェイポイントの総当たりで通れる道をチェックして、お互いのlinksにIDを登録
	void Manager::BuildWaypointLinks(float capRad, const StageBase* stage)
	{
		// 既存のリンク情報をクリア
		for (auto& wp : _waypoints) { wp.links.clear(); }

		// すべての組み合わせをチェック
		for (size_t i = 0; i < _waypoints.size(); ++i)
		{
			for (size_t j = i + 1; j < _waypoints.size(); ++j)
			{
				// 遠すぎるポイント同士はそもそも繋げない
				if (GetDistance(_waypoints[i].vPos, _waypoints[j].vPos) > 1500.0f) { continue; }

				// 間に障害物がないかチェック
				if(!CheckCapsuleLineObstacle(_waypoints[i].vPos, _waypoints[j].vPos, capRad, stage))
				{
					// なければ、お互いのIDをリンク先として登録
					_waypoints[i].links.push_back(_waypoints[j].id);
					_waypoints[j].links.push_back(_waypoints[i].id);
				}
			}
		}
	}

	// リンク情報からゴールまでの最短経路を計算する
	// 通るべきウェイポイントが順番で入ったリストを返す
	std::vector<VECTOR> Manager::FindPath(int startId, int goalId)
	{
		std::vector<VECTOR> path;
		if (_waypoints.empty() || startId < 0 || goalId < 0) { return path; }

		// 探索候補を入れるopenリスト
		std::vector<AStarNode> openList;
		// 探索済みを入れるclosedリスト
		std::vector<int> closedList;
		// ルート復元用に、全ノードの情報を保持しておくリスト
		std::vector<AStarNode> allNodes;

		// 1.スタート地点をopenリストに追加
		openList.push_back({ startId, 0.0f, GetDistance(_waypoints[startId].vPos, _waypoints[goalId].vPos), -1 });

		// 2.openリストが空になるまでループ
		while (!openList.empty())
		{
			// openリストからfCostが最小のノードを探す
			auto currentIt = openList.begin();
			for (auto it = openList.begin(); it != openList.end(); ++it)
			{
				if (it->fCost() < currentIt->fCost()) { currentIt = it; }
			}

			// そのノードを現在地として、openリストからclosedリストに移す
			AStarNode currentNode = *currentIt;
			openList.erase(currentIt);
			closedList.push_back(currentNode.id);
			allNodes.push_back(currentNode);

			// 現在地がゴールなら探索終了。ルートを復元する
			if (currentNode.id == goalId)
			{
				int traceId = currentNode.id;
				// ゴールから親ノードを逆順に辿る
				while (traceId != -1)
				{
					// 現在辿っているIDの座標をルートに追加
					path.push_back(_waypoints[traceId].vPos);

					// 履歴から、どこから来たかを探す
					auto parentIt = std::find_if(
						allNodes.begin(),
						allNodes.end(),
						[traceId](const AStarNode& n) { return n.id == traceId; }
					);

					if (parentIt != allNodes.end())
					{
						traceId = parentIt->parentId;
					}
					else
					{
						break;
					}
				}

				// リストを反転させてスタートからゴールの順にする
				std::reverse(path.begin(), path.end());
				return path;// 完成したルートを返す
			}

			// 現在地から移動できる隣のウェイポイントを全て調べる
			for (int neighborId : _waypoints[currentNode.id].links)
			{
				// 既に探索済みならスキップ
				if (std::find(closedList.begin(), closedList.end(), neighborId) != closedList.end()) { continue; }

				// 隣のポイントへ移動した場合のgCostを計算
				float newGCost = currentNode.gCost + GetDistance(_waypoints[currentNode.id].vPos, _waypoints[neighborId].vPos);

				// 隣のポイントが既にopenリストにあるかをチェック
				auto openIt = std::find_if(
					openList.begin(),
					openList.end(),
					[neighborId](const AStarNode& n) { return n.id == neighborId; }
				);

				// openリストにない、または前回のルートよりgCostが小さい場合
				if (openIt == openList.end() || newGCost < openIt->gCost)
				{
					// ノード情報を更新してopenリストに登録(上書き)
					AStarNode neighborNode = {
						neighborId,
						newGCost,
						GetDistance(_waypoints[neighborId].vPos, _waypoints[goalId].vPos),
						currentNode.id// 親は現在地
					};

					if (openIt == openList.end())
					{
						openList.push_back(neighborNode);
					}
					else
					{
						*openIt = neighborNode;
					}
				}
			}
		}

		// ゴールにたどり着けなかった場合は空のルートを返す
		return path;
	}
}
