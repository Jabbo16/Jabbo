#include "BuildingManager.hpp"
#include "RecollectManager.hpp"
#include "BuildOrderManager.hpp"

namespace
{
	auto& mapBweb = BWEB::Map::Instance();
}

namespace Jabbo
{
	BuildingManager::BuildingManager()
		= default;

	bool BuildingManager::iHaveMoney(const UnitType& unit)
	{
		int myMinerals = Broodwar->self()->minerals();
		int myGas = Broodwar->self()->gas();
		for (auto queue : instance().buildingsResourcesQueue_)
		{
			myMinerals -= queue.mineralPrice();
			myGas -= queue.gasPrice();
		}
		return myMinerals >= unit.mineralPrice() && myGas >= unit.gasPrice();
	}

	TilePosition BuildingManager::chooseGeyser()
	{
		return TilePositions::Invalid;
	}

	BuildingManager& BuildingManager::instance()
	{
		static BuildingManager instance;
		return instance;
	}

	void BuildingManager::onUnitCreate(const Unit unit)
	{
		for (auto& builder : instance().workerBuild_)
		{
			if (builder.second.first == unit->getType() && builder.second.second == unit->getTilePosition())
			{
				if (Broodwar->self()->getRace() == Races::Terran)
				{
					instance().workerTask_.insert(pair<Unit, Unit>{unit, builder.first});
				}
				else
				{
					instance().unitTransforming_.insert(unit);
					RecollectManager::Instance().workerIdle_.insert(builder.first);
				}
				for (const auto type : instance().buildingsResourcesQueue_)
				{
					if (unit->getType() == type)
					{
						instance().buildingsResourcesQueue_.remove(type);
						break;
					}
				}
				instance().workerBuild_.erase(builder.first);
				break;
			}
		}
	}

	void BuildingManager::onUnitComplete(const Unit unit)
	{
		if (Broodwar->self()->getRace() == Races::Terran)
		{
			if (instance().workerTask_.find(unit) != instance().workerTask_.end())
			{
				RecollectManager::Instance().workerIdle_.insert(instance().workerTask_[unit]);
				// TODO insertar en lista de edificios
				instance().workerTask_.erase(unit);
			}
		}
		else
		{
			if (instance().unitTransforming_.find(unit) != instance().unitTransforming_.end())
			{
				// TODO insertar en lista de edificios
				instance().unitTransforming_.erase(unit);
			}
		}
	}

	void BuildingManager::onFrame()
	{
		for (auto& builder : instance().workerBuild_)
		{
			if (builder.first->getOrder() == Orders::PlaceBuilding)
			{
				continue;
			}
			if (Broodwar->isVisible(builder.second.second))
			{
				builder.first->build(builder.second.first, builder.second.second);
			}
		}
		if (Broodwar->self()->minerals() < 100)
		{
			return;
		}
		auto chosenType = UnitTypes::Unknown;
		auto isFromBO = false;
		if (!BuildOrderManager::instance().myBo.itemsBO.empty())
		{
			BOItem next = BuildOrderManager::instance().myBo.itemsBO[0];

			if (!next.unit.isBuilding())
			{
				return;
			}
			if (iHaveMoney(next.unit) && Broodwar->self()->supplyUsed() / 2 >= next.supply)
			{
				chosenType = next.unit;
				isFromBO = true;
			}
		}
		else
		{
			if (Broodwar->self()->supplyTotal() / 2 - Broodwar->self()->supplyUsed() / 2 <= 2 && Broodwar->self()->getRace() != Races::Zerg)
			{
				for (const auto item : instance().buildingsResourcesQueue_)
				{
					if (item == Broodwar->self()->getRace().getSupplyProvider()) return;
				}
				if (Broodwar->self()->getRace() == Races::Terran)
				{
					for (const auto task : instance().workerTask_)
					{
						if (task.first->getType() == Broodwar->self()->getRace().getSupplyProvider()) return;
					}
				}
				else
				{
					for (const auto transforming : instance().unitTransforming_)
					{
						if (transforming->getType() == Broodwar->self()->getRace().getSupplyProvider()) return;
					}
				}
				chosenType = Broodwar->self()->getRace().getSupplyProvider();
			}
		}

		if (chosenType == UnitTypes::Unknown)
		{
			return;
		}

		auto chosenPosition = TilePositions::Unknown;
		if (!chosenType.isRefinery())
		{
			if (chosenType.requiresPsi())
			{
				chosenPosition = mapBweb.getBuildPosition(chosenType, instance().reserved_, Broodwar->self()->getStartLocation(),
					true);
			}
			else
			{
				chosenPosition = mapBweb.getBuildPosition(chosenType, instance().reserved_, Broodwar->self()->getStartLocation(),
					false);
			}
		}
		else
		{
			chosenPosition = instance().chooseGeyser();
		}
		Unit chosenWorker = nullptr;
		for (auto unit : RecollectManager::Instance().workerIdle_)
		{
			if (!unit->isIdle())
			{
				continue;
			}
			if (!chosenWorker || unit->getDistance(Position(chosenPosition)) < chosenWorker->getDistance(
				Position(chosenPosition)))
			{
				chosenWorker = unit;
			}
		}
		if (chosenWorker)
		{
			RecollectManager::Instance().workerIdle_.erase(chosenWorker);
			pair<UnitType, TilePosition> placeType = { chosenType, chosenPosition };
			instance().workerBuild_.insert(pair<Unit, pair<UnitType, TilePosition>>(chosenWorker, placeType));
			instance().buildingsResourcesQueue_.emplace_back(chosenType);
			if (isFromBO)
			{
				instance().itemsInProgress.emplace_back(BuildOrderManager::instance().myBo.itemsBO[0]);
				BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
			}
			chosenWorker->move(Position(chosenPosition));
		}
	}
}