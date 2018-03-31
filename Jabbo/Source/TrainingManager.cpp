#include "TrainingManager.hpp"
#include <set>
#include "BuildingManager.hpp"
#include "UnitInfoManager.h"
#include "ResourceManager.hpp"
#include "EconManager.hpp"
#include "InfoManager.hpp"
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

	void TrainingManager::updateScoring()
	{
		// Unit score based off enemy composition
		auto enemyUnitInfo = UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->enemy());
		for (const auto t : enemyUnitInfo.getUnits())
		{
			// For each type, add a score to production based on the unit count divided by our current unit count
			if (InfoManager::instance().myRace == Races::Protoss)
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
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Medic:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Firebat:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Terran_Vulture:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.10) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.90) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Goliath:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.85) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.15) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.85) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.15) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Arbiter] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Arbiter)));
			break;
		case UnitTypes::Enum::Terran_Wraith:
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Science_Vessel:
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Battlecruiser:
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			break;
		case UnitTypes::Enum::Terran_Valkyrie:
			break;

		case UnitTypes::Enum::Zerg_Zergling:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Hydralisk:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.50) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Lurker:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Ultralisk:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 0.25) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 0.75) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Zerg_Mutalisk:
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Guardian:
			unitScore_[UnitTypes::Protoss_Dragoon] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Dragoon)));
			unitScore_[UnitTypes::Protoss_Corsair] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Corsair)));
			break;
		case UnitTypes::Enum::Zerg_Devourer:
			break;
		case UnitTypes::Enum::Zerg_Defiler:
			unitScore_[UnitTypes::Protoss_Zealot] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Zealot)));
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;

		case UnitTypes::Enum::Protoss_Zealot:
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Dragoon:
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_High_Templar:
			unitScore_[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Dark_Templar:
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Reaver:
			unitScore_[UnitTypes::Protoss_Reaver] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Reaver)));
			break;
		case UnitTypes::Enum::Protoss_Archon:
			unitScore_[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Dark_Archon:
			unitScore_[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Scout:
			break;
		case UnitTypes::Enum::Protoss_Carrier:
			unitScore_[UnitTypes::Protoss_Scout] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_Scout)));
			break;
		case UnitTypes::Enum::Protoss_Arbiter:
			unitScore_[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		case UnitTypes::Enum::Protoss_Corsair:
			unitScore_[UnitTypes::Protoss_High_Templar] += (count * unit.supplyRequired() * 1.00) / max(1.0, double(Broodwar->self()->visibleUnitCount(UnitTypes::Protoss_High_Templar)));
			break;
		}
	}

	void TrainingManager::onFrame()
	{
		//Workers
		auto myUnitData = UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self());
		if (EconManager::iHaveMoney(InfoManager::instance().myRace.getWorker()) && unsigned int(Broodwar->self()->allUnitCount(InfoManager::instance().myRace.getWorker())) < unsigned int(ResourceManager::instance().minerals.size() * 2 + ResourceManager::instance().gas.size() + 1))
		{
			for (auto u : myUnitData.getUnits())
			{
				if (u.second.type.isResourceDepot() && !u.first->isTraining())
				{
					switch (InfoManager::instance().myRace)
					{
					case Races::Terran:
						u.first->train(UnitTypes::Terran_SCV);
					case Races::Zerg:
						for (auto uLarva : u.first->getLarva())
						{
							uLarva->morph(UnitTypes::Zerg_Drone);
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
			auto next = BuildOrderManager::instance().myBo.itemsBO[0];
			if (!std::holds_alternative<UnitType>(next.type))
			{
				return;
			}
			UnitType aux = std::get<UnitType>(next.type);
			if (!aux.isBuilding() && (EconManager::iHaveMoney(aux) && Broodwar->self()->supplyUsed() / 2 >= next.supply))
			{
				for (auto &b : myUnitData.getUnits())
				{
					if (b.first->canTrain(aux) && !b.first->isTraining())
					{
						b.first->train(aux);
						const auto frame = aux.buildTime() + Broodwar->getFrameCount();
						instance().unitQueue.emplace_back(UnitItemQueue{ b.first, aux, frame, true });
						BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
						return;
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
		if (highestType != UnitTypes::None && EconManager::iHaveMoney(highestType) && building)
		{
			building->train(highestType);
			auto const frame = highestType.buildTime() + Broodwar->getFrameCount();
			instance().unitQueue.emplace_back(UnitItemQueue{ building, highestType, frame, false });
			// TODO Remove Item from queue at onCreate() or onComplete() and OnDestroy()
		}
	}
	bool TrainingManager::hasUnitTypeRequirement(UnitType unitType)
	{
		auto requirements = unitType.requiredUnits();
		for (const auto requirement : requirements)
		{
			auto found = false;
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