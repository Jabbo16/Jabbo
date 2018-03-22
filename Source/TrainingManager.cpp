#include "TrainingManager.hpp"
#include <set>
#include "BuildingManager.hpp"
using namespace BWAPI;
using namespace std;
namespace Jabbo {
	TrainingManager::TrainingManager()
		= default;

	TrainingManager & TrainingManager::Instance()
	{
		static TrainingManager instance;
		return instance;
	}

	bool TrainingManager::iHaveMoney(const UnitType & unit)
	{
		auto myMinerals = Broodwar->self()->minerals();
		auto myGas = Broodwar->self()->gas();
		for (auto queue : BuildingManager::instance().buildingsResourcesQueue)
		{
			myMinerals -= queue.mineralPrice();
			myGas -= queue.gasPrice();
		}
		return myMinerals >= unit.mineralPrice() && myGas >= unit.gasPrice();
	}
	void TrainingManager::onFrame()
	{
		//		if (Broodwar->self()->supplyTotal() / 2 - Broodwar->self()->supplyUsed() / 2 <= 2)
		//		{
		//			return;
		//		}

		//Workers
		for (auto u : Broodwar->self()->getUnits())
		{
			if (u->getType().isResourceDepot() && !u->isTraining())
			{
				switch (Broodwar->self()->getRace())
				{
				case Races::Terran:
					u->train(UnitTypes::Terran_SCV);
				case Races::Zerg:
					for (Unit uLarva : u->getLarva())
					{
						u->train(UnitTypes::Zerg_Drone);
					}
				case Races::Protoss:
					u->train(UnitTypes::Protoss_Probe);
				default:;
				}
			}
		}
	}
}