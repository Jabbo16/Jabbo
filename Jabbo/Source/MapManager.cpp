#include "MapManager.hpp"
#include <bwem.h>
using namespace BWAPI;
namespace { auto & bwem = BWEM::Map::Instance(); }

namespace Jabbo {
	MapManager::MapManager()
		= default;

	vector<Position> MapManager::getAdjacentPos(const Position pos)
	{
		vector<Position> positions{};
		if ((pos + Position(0, 1)).isValid()) positions.emplace_back(pos + Position(0, 1));
		if ((pos + Position(0, -1)).isValid()) positions.emplace_back(pos + Position(0, -1));
		if ((pos + Position(1, 0)).isValid()) positions.emplace_back(pos + Position(1, 0));
		if ((pos + Position(1, -1)).isValid()) positions.emplace_back(pos + Position(1, -1));
		if ((pos + Position(1, 1)).isValid()) positions.emplace_back(pos + Position(1, 1));
		if ((pos + Position(-1, 1)).isValid()) positions.emplace_back(pos + Position(-1, 1));
		if ((pos + Position(-1, 0)).isValid()) positions.emplace_back(pos + Position(-1, 0));
		if ((pos + Position(-1, -1)).isValid()) positions.emplace_back(pos + Position(-1, -1));
		return positions;
	}

	MapManager & MapManager::instance()
	{
		static MapManager instance;
		return instance;
	}

	std::pair<double, double> operator +(const std::pair<double, double>& x, const std::pair<double, double>& y) {
		return pair<double, double>(x.first + y.first, x.second + y.second);
	}

	pair<double, double> MapManager::sumPosition(const vector<pair<double, double>>& vectors)
	{
		pair<double, double> sum{};
		for (const auto& vector : vectors)
		{
			sum = sum + vector;
		}
		return sum;
	}

	double MapManager::broodWarDistance(const pair<double, double> pos1, const pair<double, double> pos2) const
	{
		const auto dx = abs(pos1.first - pos2.first);
		const auto dy = abs(pos1.second - pos2.second);
		const auto d = min(dx, dy);
		const auto D = max(dx, dy);
		if (d < D / 4) {
			return D;
		}
		return D - D / 16 + d * 3 / 8 - D / 64 + d * 3 / 256;
	}

	double MapManager::broodWarDistance(const Position pos1, const Position pos2) const
	{
		const auto dx = abs(pos1.x - pos2.x);
		const auto dy = abs(pos1.y - pos2.y);
		const auto d = min(dx, dy);
		const auto D = max(dx, dy);
		if (d < D / 4) {
			return D;
		}
		return D - D / 16 + d * 3 / 8 - D / 64 + d * 3 / 256;
	}

	double MapManager::getGroundDistance(Position start, Position end)
	{
		/*int* dist = 0;
		const auto& path = bwem.GetPath(start, end, dist);
		return *dist == -1 ? DBL_MAX : static_cast<double>(*dist);*/
		auto dist = 0.0;
		if (!start.isValid() || !end.isValid() || !bwem.GetArea(WalkPosition(start)) || !bwem.GetArea(WalkPosition(end))) return DBL_MAX;
		for (const auto& cpp : bwem.GetPath(start, end))
		{
			const auto center = Position{ cpp->Center() };
			dist += start.getDistance(center);
			start = center;
		}
		return dist += start.getDistance(end);
	}

	Position MapManager::getKitePosition(set<scutil::UnitInfo> &enemiesToKite, const Unit myUnit)
	{
		if (enemiesToKite.empty()) {
			return Positions::None;
		}
		const auto ownPosition = myUnit->getPosition();

		vector<pair<double, double>> vectors{};

		auto minDistance = DBL_MAX;
		for (const auto& enemy : enemiesToKite) {
			const auto enemyPosition = enemy.lastPosition;
			auto difference = ownPosition - enemyPosition;
			pair<double, double> unitV{ difference.x , difference.y };
			const auto distance = ownPosition.getDistance(enemyPosition);
			if (distance < minDistance) {
				minDistance = distance;
			}
			unitV.first = (1.0 / distance) * unitV.first;
			unitV.second = (1.0 / distance) * unitV.second;
			vectors.emplace_back(unitV);
		}
		minDistance = 2.0 * minDistance * minDistance;
		for (auto &vector : vectors) {
			vector.first *= minDistance;
			vector.second *= minDistance;
		}
		const auto sumAll = sumPosition(vectors);
		return ownPosition + Position(int(sumAll.first / vectors.size()), int(sumAll.second / vectors.size()));
	}
}