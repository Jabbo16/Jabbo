#pragma once
#include <BWAPI.h>
#include <UnitInfoManager.h>
#include <set>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class MapManager
	{
	public:
		MapManager();
		static vector<Position> getAdjacentPos(Position pos);
		static MapManager & instance();
		static pair<double, double> sumPosition(const vector<pair<double, double>>& vectors);
		double broodWarDistance(pair<double, double>, pair<double, double>) const;
		double broodWarDistance(const Position pos1, const Position pos2) const;
		static double getGroundDistance(Position start, Position end);
		static Position getKitePosition(set<scutil::UnitInfo> &enemiesToKite, Unit myUnit);
	};
}