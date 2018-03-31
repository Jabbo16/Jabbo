#pragma once
#include <BWAPI.h>
#include "BuildOrderManager.hpp"
#include "BrainTree/BrainTree.h"
namespace BT = BrainTree;

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class BuildingManager
	{
		struct BuildInfo
		{
			UnitType type = UnitTypes::Unknown;
			TilePosition pos = TilePositions::Unknown;
			bool isFromBO = false;
		};
		class ChoosePlace : public BT::Leaf {};
		class ChooseType : public BT::Leaf {};
		TilePosition chosenPosition_;
		map<Unit, BOItem > itemsInProgress_{};
		UnitType chosenType_ = UnitTypes::Unknown;
		bool isFromBO_ = false;
		map<Unit, BuildInfo> unitTransforming_{};
		BT::BehaviorTree buildingTree_{};

	public:
		BuildingManager();

		map < Unit, BuildInfo> workerBuild{};
		map <Unit, Unit> workerTask{};
		list<UnitType> buildingsResourcesQueue{};
		static TilePosition chooseGeyser();
		static void onFrame();
		static BuildingManager & instance();
		static void onUnitCreate(Unit);
		static void onUnitComplete(Unit);
		static void onUnitDestroy(Unit);
		static void initTree();
	};
}