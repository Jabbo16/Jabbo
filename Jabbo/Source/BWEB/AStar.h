#pragma once
#include "BWEB.h"

namespace BWEB
{
	using uint = unsigned int;

	using namespace BWAPI;
	using namespace std;

	struct Node
	{
		uint G, H;
		TilePosition coordinates, direction;
		Node *parent;

		Node(TilePosition tile, Node *parent = nullptr);
		uint getScore() const;
	};
	struct QueueObj {
		QueueObj(BWAPI::TilePosition const pos, int const dist, int const effectiveDist) : pos{ pos }, dist{ dist }, effectiveDist{ effectiveDist } { }
		mutable BWAPI::TilePosition pos;
		mutable int dist;
		mutable set<TilePosition> path{};
		int effectiveDist;

		bool operator<(QueueObj const &other) const {
			return effectiveDist < other.effectiveDist;
		}
	};
	class AStar
	{
		static Node* findNodeOnList(set<Node*>&, TilePosition);
		vector<TilePosition> direction, walls;
		uint directions;
		uint manhattan(TilePosition, TilePosition) const;

	public:
		std::set<TilePosition> pathfind(BWAPI::TilePosition topLeftStart, BWAPI::TilePosition const topLeftEnd) const;
		AStar();
		vector<TilePosition> findPath(BWEM::Map&, BWEB::Map&, TilePosition, TilePosition, bool);
	};
}
