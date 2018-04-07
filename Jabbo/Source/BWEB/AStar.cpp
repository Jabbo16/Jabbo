#include "AStar.h"

using namespace std::placeholders;

namespace BWEB
{
	std::set<TilePosition> AStar::pathfind(BWAPI::TilePosition topLeftStart, BWAPI::TilePosition const topLeftEnd) const
	{
		struct mapSize
		{
			int x;
			int y;
		};
		const auto manhattanDistance = [](BWAPI::TilePosition const &a, BWAPI::TilePosition const &b) {
			return abs(a.x - b.x) + abs(a.y - b.y);
		};

		const std::vector<BWAPI::TilePosition> deltas{ {0, 1}, {0, -1}, {1, 0}, {-1, 0} };
		const mapSize mapSize{ Broodwar->mapWidth() ,Broodwar->mapHeight() };
		const auto totalSize = mapSize.x * mapSize.y;
		set<TilePosition> visitedPPAP;
		std::priority_queue<QueueObj> queue;
		set<TilePosition> realPath;
		queue.emplace(topLeftStart, 0, 0);
		visitedPPAP.insert(topLeftStart);
		while (!queue.empty()) {
			auto const current = queue.top();
			queue.pop();

			if (current.pos == topLeftEnd) {
				return current.path;
			}

			if (visitedPPAP.find(current.pos) != visitedPPAP.end()) continue;

			visitedPPAP.insert(current.pos);
			for (auto const &d : deltas) {
				auto const next = current.pos + d;
				if (0 <= next.x && next.x < mapSize.x
					&& 0 <= next.y && next.y < mapSize.y) {
					if (visitedPPAP.find(next) != visitedPPAP.end()) continue;
					if (!next.isValid() || BWEB::Map::Instance().overlapGrid[next.x][next.y] > 0 || !BWEB::Map::isWalkable(next)) continue;
					if (BWEB::Map::Instance().overlapsCurrentWall(next) != UnitTypes::None) continue;
					if (BWEM::Map::Instance().GetArea(next) && BWEM::Map::Instance().GetArea(next) != BWEM::Map::Instance().GetArea(TilePosition(topLeftStart)) && BWEM::Map::Instance().GetArea(next) != BWEM::Map::Instance().GetArea(TilePosition(topLeftEnd))) continue;
					QueueObj temp(current.pos, current.dist + 1, current.dist + manhattanDistance(next, topLeftEnd) + 1);
					temp.path = current.path;
					queue.emplace(temp);
				}
			}
		}
		return realPath;
	}

	Node::Node(const TilePosition tile, Node *newParent)
	{
		parent = newParent;
		coordinates = tile;
		G = H = 0;
	}

	uint Node::getScore() const
	{
		return G + H;
	}

	AStar::AStar() : directions(0)
	{
		direction =
		{
			{0, 1}, {1, 0}, {0, -1}, {-1, 0},
			{-1, -1}, {1, 1}, {-1, 1}, {1, -1}
		};
	}

	vector<TilePosition> AStar::findPath(BWEM::Map& bwem, BWEB::Map& map, const TilePosition source, const TilePosition target, bool walling)
	{
		Node *current = nullptr;
		set<Node*> openSet, closedSet;
		openSet.insert(new Node(source));
		directions = 4;

		while (!openSet.empty())
		{
			current = *openSet.begin();
			for (auto node : openSet)
			{
				if (node->getScore() <= current->getScore())
					current = node;
			}

			if (current->coordinates == target) break;

			closedSet.insert(current);
			openSet.erase(find(openSet.begin(), openSet.end(), current));

			for (uint i = 0; i < directions; ++i)
			{
				auto tile(current->coordinates + direction[i]);

				// Detection collision or skip tiles already added to closed set
				if (!tile.isValid() || map.overlapGrid[tile.x][tile.y] > 0 || !BWEB::Map::isWalkable(tile) || findNodeOnList(closedSet, tile)) continue;
				if (map.overlapsCurrentWall(tile) != UnitTypes::None) continue;
				if (bwem.GetArea(tile) && bwem.GetArea(tile) != bwem.GetArea(source) && bwem.GetArea(tile) != bwem.GetArea(target)) continue;

				// Cost function?
				const auto totalCost = current->G + ((i < 4) ? 10 : 14);

				// Checks if the node has been made already, if not it creates one
				auto successor = findNodeOnList(openSet, tile);
				if (successor == nullptr)
				{
					successor = new Node(tile, current);
					successor->G = totalCost;
					successor->H = manhattan(successor->coordinates, target);
					successor->direction = direction[i];
					openSet.insert(successor);
				}
				// If the node exists, update its cost and parent node
				else if (totalCost < successor->G)
				{
					successor->parent = current;
					successor->G = totalCost;
				}
			}
		}

		vector<TilePosition> path;
		if (current->coordinates != target)
			return path;

		while (current != nullptr)
		{
			path.push_back(current->coordinates);
			current = current->parent;
		}

		openSet.clear();
		closedSet.clear();
		return path;
	}

	Node* AStar::findNodeOnList(set<Node*>& nodes, const TilePosition coordinates)
	{
		for (auto node : nodes) {
			if (node->coordinates == coordinates) {
				return node;
			}
		}
		return nullptr;
	}

	uint AStar::manhattan(const TilePosition source, const TilePosition target) const
	{
		return abs(source.x - target.x) + abs(source.y - target.y);
	}
}