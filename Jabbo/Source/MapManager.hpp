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
		static MapManager & instance();
		static pair<double, double> sumPosition(const vector<pair<double, double>>& pairs);
		static double getGroundDistance(Position start, Position end);
		static Position getKitePosition(set<scutil::UnitInfo> &enemiesToKite, Unit myUnit);
	};
}