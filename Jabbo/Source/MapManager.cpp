#include "MapManager.hpp"
#include "BWEM 1.4.1/src/bwem.h"
using namespace BWAPI;
namespace { auto & bwem = BWEM::Map::Instance(); }
namespace Jabbo {
	MapManager::MapManager()
		= default;

	MapManager & MapManager::instance()
	{
		static MapManager instance;
		return instance;
	}

	std::pair<double, double> operator +(const std::pair<double, double>& x, const std::pair<double, double>& y) {
		return std::make_pair(x.first + y.first, x.second + y.second);
	}

	pair<double, double> MapManager::sumPosition(const vector<pair<double, double>>& vectors)
	{
		pair<double, double> sum{};
		for (const auto vector : vectors)
		{
			sum = sum + vector;
		}
		return sum;
	}

	double MapManager::getGroundDistance(Position start, Position end)
	{
		auto dist = 0.0;
		if (!start.isValid() || !end.isValid() || !BWEM::Map::Instance().GetArea(WalkPosition(start)) || !BWEM::Map::Instance().GetArea(WalkPosition(end)))
			return DBL_MAX;

		for (auto& cpp : BWEM::Map::Instance().GetPath(start, end))
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

		double minDistance = 99999999;
		for (const auto enemy : enemiesToKite) {
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