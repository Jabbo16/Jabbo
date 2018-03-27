#pragma once
#include <BWAPI.h>
#include "BWEB/BWEB.h"
#include "BuildOrderManager.hpp"
#include "BrainTree.h"
namespace bt = BrainTree;
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class BuildingManager
	{
		struct buildInfo
		{
			UnitType type = UnitTypes::Unknown;
			TilePosition pos = TilePositions::Unknown;
			bool isFromBO = false;
		};
		class ChoosePlace : public bt::Leaf {};
		class ChooseType : public bt::Leaf {};
		TilePosition chosenPosition_;
		map<Unit, BOItem > itemsInProgress_;
		UnitType chosenType_ = UnitTypes::Unknown;
		bool isFromBO_ = false;
		map<Unit, buildInfo> unitTransforming_;
		set<TilePosition> reserved_;
		static bool iHaveMoney(const UnitType& unit);
		bt::BehaviorTree buildingTree_;

	public:
		BuildingManager();

		map < Unit, buildInfo> workerBuild;
		map <Unit, Unit> workerTask;
		list<UnitType> buildingsResourcesQueue;
		static TilePosition chooseGeyser();
		static void onFrame();
		static BuildingManager & instance();
		static void onUnitCreate(Unit);
		static void onUnitComplete(Unit);
		static void onUnitDestroy(Unit);
		static void initTree();
	};
}