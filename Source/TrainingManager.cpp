#include "TrainingManager.hpp"
#include <set>
#include "BuildingManager.hpp"
#include "UnitInfoManager.h"
#include "ResourceManager.hpp"
using namespace scutil;
using namespace Filter;
using namespace BWAPI;
using namespace std;
namespace Jabbo {
	TrainingManager::TrainingManager()
		= default;

	TrainingManager & TrainingManager::instance()
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

	void TrainingManager::updateScoring()
	{
		// Unit score based off enemy composition
		auto enemyUnitInfo = UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->enemy());
		for (const auto t : enemyUnitInfo.getUnits())
		{
			// For each type, add a score to production based on the unit count divided by our current unit count
			if (Broodwar->self()->getRace() == Races::Protoss)
			{
				updateProtossUnitScore(t.second.type, enemyUnitInfo.getNumUnits(t.second.type));
			}
		}
	}

	void TrainingManager::updateProtossUnitScore(UnitType unit, const int count)
	{
		switch (unit)
		{
		case UnitTypes::Enum::Terran_Marine:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Medic:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Firebat:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Vulture:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.10) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.90) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Goliath:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.85) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.15) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.85) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.15) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Wraith:
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Science_Vessel:
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Battlecruiser:
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Valkyrie:
			break;

		case UnitTypes::Enum::Zerg_Zergling:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Hydralisk:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Lurker:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Ultralisk:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Mutalisk:
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Guardian:
			unitScore[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Devourer:
			break;
		case UnitTypes::Enum::Zerg_Defiler:
			unitScore[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;

		case UnitTypes::Enum::Protoss_Zealot:
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Dragoon:
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_High_Templar:
			unitScore[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Dark_Templar:
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Reaver:
			unitScore[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Archon:
			unitScore[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Dark_Archon:
			unitScore[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Scout:
			break;
		case UnitTypes::Enum::Protoss_Carrier:
			unitScore[UnitTypes::Protoss_Scout] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Scout)));
			break;
		case UnitTypes::Enum::Protoss_Arbiter:
			unitScore[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Corsair:
			unitScore[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		}
	}

	void TrainingManager::onFrame()
	{
		//Workers
		auto myUnitData = UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self());
		if (unsigned int(Broodwar->self()->allUnitCount(Broodwar->self()->getRace().getWorker())) < unsigned int(ResourceManager::instance().minerals.size() * 2 + ResourceManager::instance().gas.size() + 1))
		{
			for (auto u : myUnitData.getUnits())
			{
				if (u.second.type.isResourceDepot() && !u.first->isTraining())
				{
					switch (Broodwar->self()->getRace())
					{
					case Races::Terran:
						u.first->train(UnitTypes::Terran_SCV);
					case Races::Zerg:
						for (Unit uLarva : u.first->getLarva())
						{
							u.first->train(UnitTypes::Zerg_Drone);
						}
					case Races::Protoss:
						u.first->train(UnitTypes::Protoss_Probe);
					default:;
					}
				}
			}
		}

		if (!BuildOrderManager::instance().myBo.itemsBO.empty())
		{
			BOItem next = BuildOrderManager::instance().myBo.itemsBO[0];

			if (!next.unit.isBuilding())
			{
				if (instance().iHaveMoney(next.unit) && Broodwar->self()->supplyUsed() / 2 >= next.supply)
				{
					for (auto &b : myUnitData.getUnits())
					{
						if (b.first->canTrain(next.unit) && !b.first->isTraining())
						{
							b.first->train(next.unit);
							const auto frame = next.unit.buildTime() + Broodwar->getFrameCount();
							instance().unitQueue.emplace_back(unitItemQueue{ b.first, next.unit, frame, true });
							BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
							return;
						}
					}
				}
			}
		}
		instance().updateScoring();
		Unit building = nullptr;
		auto highestType = UnitTypes::None;
		for (auto &b : myUnitData.getUnits())
		{
			if (!b.second.type.isBuilding() || b.second.type.isResourceDepot() || !b.first->isIdle()) {
				continue;
			}
			auto highestPriority = 0.0;

			for (auto &unit : b.second.type.buildsWhat())
			{
				if (!b.first->canTrain(unit))
				{
					continue;
				}
				if (instance().getUnitScore()[unit] >= highestPriority)
				{
					highestPriority = instance().getUnitScore()[unit];
					highestType = unit;
					building = b.first;
				}
			}
		}
		//Broodwar->sendText(highestType.c_str());
		if (highestType != UnitTypes::None && iHaveMoney(highestType) && building)
		{
			building->train(highestType);
			auto const frame = highestType.buildTime() + Broodwar->getFrameCount();
			instance().unitQueue.emplace_back(unitItemQueue{ building, highestType, frame, false });
			// TODO Remove Item from queue at onCreate() or onComplete() and OnDestroy()
		}
	}
	bool TrainingManager::hasUnitTypeRequirement(BWAPI::UnitType unit_type)
	{
		auto requirements = unit_type.requiredUnits();
		for (const auto requirement : requirements)
		{
			bool found = false;
			for (const auto building : UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self()).getUnits())
			{
				if (building.second.type == requirement.first &&
					building.first->isCompleted())
				{
					found = true;
				}
				if (found)
					break;
			}
			if (!found)
				return false;
		}
		return true;
	}
}