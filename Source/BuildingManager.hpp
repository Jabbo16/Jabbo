#pragma once
#include <BWAPI.h>
#include "BWEB/BWEB.h"
#include "BuildOrderManager.hpp"

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class BuildingManager
	{
		set<Unit> unitTransforming_;
		vector< BOItem > itemsInProgress; //TODO remove item when its completed/created
		set<TilePosition> reserved_;
	public:
		BuildingManager();
		list<UnitType> buildingsResourcesQueue_;
		map < Unit, pair<UnitType, TilePosition >> workerBuild_;
		map <Unit, Unit> workerTask_;
		static bool iHaveMoney(const UnitType& unit);
		static TilePosition chooseGeyser();
		static void onFrame();
		static BuildingManager & instance();
		static void onUnitCreate(Unit);
		static void onUnitComplete(Unit);
	};
}