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
		class ChoosePlace : public bt::Leaf {};
		class ChooseType : public bt::Leaf {};
		TilePosition chosenPosition_;
		vector< BOItem > itemsInProgress_; //TODO remove item when its completed/created
		UnitType chosenType_ = UnitTypes::Unknown;
		bool isFromBO_ = false;
		set<Unit> unitTransforming_;
		set<TilePosition> reserved_;
		static bool iHaveMoney(const UnitType& unit);
		bt::BehaviorTree buildingTree_;

	public:
		BuildingManager();

		map < Unit, pair<UnitType, TilePosition >> workerBuild;
		map <Unit, Unit> workerTask;
		list<UnitType> buildingsResourcesQueue;
		static TilePosition chooseGeyser();
		static void onFrame();
		static BuildingManager & instance();
		static void onUnitCreate(Unit);
		static void onUnitComplete(Unit);
		static void initTree();
	};
}