#include "MapManager.hpp"
#include "BWEM 1.4.1/src/gridMap.h"
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

	std::pair<double, double> operator +(const std::pair<int, int>& x, const std::pair<double, double>& y) {
		return std::make_pair(x.first + y.first, x.second + y.second);
	}

	pair<double, double> MapManager::sumPosition(const vector<pair<double, double>>& vectors)
	{
		pair<double, double> sum{ 0,0 };
		for (const auto vector : vectors)
		{
			sum = sum + vector;
		}
		return sum;
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
			unitV.first = (1 / distance) * unitV.first;
			unitV.second = (1 / distance) * unitV.second;
			vectors.emplace_back(unitV);
		}
		minDistance = 2 * minDistance * minDistance;
		for (auto &vector : vectors) {
			vector.first *= minDistance;
			vector.second *= minDistance;
		}
		const auto sumAll = sumPosition(vectors);
		return ownPosition + Position(int(sumAll.first / vectors.size()), int(sumAll.second / vectors.size()));
	}
}