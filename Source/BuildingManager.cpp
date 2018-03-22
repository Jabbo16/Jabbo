#include "BuildingManager.hpp"
#include "RecollectManager.hpp"
#include "BuildOrderManager.hpp"
#include "BaseManager.hpp"
#include "ResourceManager.hpp"

namespace
{
	auto& mapBweb = BWEB::Map::Instance();
}

namespace Jabbo
{
	BuildingManager::BuildingManager() = default;

	bool BuildingManager::iHaveMoney(const UnitType& unit)
	{
		int myMinerals = Broodwar->self()->minerals();
		int myGas = Broodwar->self()->gas();
		for (auto queue : instance().buildingsResourcesQueue)
		{
			myMinerals -= queue.mineralPrice();
			myGas -= queue.gasPrice();
		}
		return myMinerals >= unit.mineralPrice() && myGas >= unit.gasPrice();
	}

	TilePosition BuildingManager::chooseGeyser()
	{
		for (auto base : BaseManager::instance().bases)
		{
			if (base.owner != Ally || base.geysers.empty())
			{
				continue;
			}
			for (auto g : base.geysers)
			{
				if (!g.second)
				{
					base.geysers[g.first] = true;
					return g.first->getTilePosition();
				}
			}
		}
		return TilePositions::Unknown;
	}

	BuildingManager& BuildingManager::instance()
	{
		static BuildingManager instance;
		return instance;
	}

	void BuildingManager::onUnitCreate(const Unit unit)
	{
		for (auto& builder : instance().workerBuild)
		{
			if (builder.second.first == unit->getType() && builder.second.second == unit->getTilePosition())
			{
				if (Broodwar->self()->getRace() == Races::Terran)
				{
					instance().workerTask.insert(pair<Unit, Unit>{unit, builder.first});
				}
				else
				{
					instance().unitTransforming_.insert(unit);
					RecollectManager::instance().workerIdle.insert(builder.first);
				}
				for (const auto type : instance().buildingsResourcesQueue)
				{
					if (unit->getType() == type)
					{
						instance().buildingsResourcesQueue.remove(type);
						break;
					}
				}
				instance().workerBuild.erase(builder.first);
				break;
			}
		}
	}

	void BuildingManager::onUnitComplete(const Unit unit)
	{
		if (unit->getType().isRefinery())
		{
			ResourceManager::instance().gas.insert(std::pair<Unit, int>(unit, 0));
		}
		if (Broodwar->self()->getRace() == Races::Terran)
		{
			if (instance().workerTask.find(unit) != instance().workerTask.end())
			{
				RecollectManager::instance().workerIdle.insert(instance().workerTask[unit]);
				// TODO insertar en lista de edificios
				instance().workerTask.erase(unit);
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

	void BuildingManager::onUnitDestroy(Unit)
	{
	}

	void BuildingManager::initTree()
	{
		class ChooseType : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					instance().chosenType_ = UnitTypes::Unknown;
					instance().isFromBO_ = false;
					if (!BuildOrderManager::instance().myBo.itemsBO.empty())
					{
						BOItem next = BuildOrderManager::instance().myBo.itemsBO[0];

						if (!next.unit.isBuilding())
						{
							return Status::Failure;
						}
						if (instance().iHaveMoney(next.unit) && Broodwar->self()->supplyUsed() / 2 >= next.supply)
						{
							instance().chosenType_ = next.unit;
							instance().isFromBO_ = true;
						}
					}
					else
					{
						if (Broodwar->self()->supplyTotal() / 2 - Broodwar->self()->supplyUsed() / 2 <= 2 && Broodwar->self()->getRace() != Races::Zerg)
						{
							if (!instance().iHaveMoney(Broodwar->self()->getRace().getSupplyProvider()))
							{
								return Status::Failure;
							}
							for (const auto item : instance().buildingsResourcesQueue)
							{
								if (item == Broodwar->self()->getRace().getSupplyProvider()) return Status::Failure;
							}
							if (Broodwar->self()->getRace() == Races::Terran)
							{
								for (const auto task : instance().workerTask)
								{
									if (task.first->getType() == Broodwar->self()->getRace().getSupplyProvider()) return Status::Failure;
								}
							}
							else
							{
								for (const auto transforming : instance().unitTransforming_)
								{
									if (transforming->getType() == Broodwar->self()->getRace().getSupplyProvider()) return Status::Failure;
								}
							}
							instance().chosenType_ = Broodwar->self()->getRace().getSupplyProvider();
						}
					}
					if (instance().chosenType_ != UnitTypes::Unknown)
					{
						return Status::Success;
					}
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};
		class ChoosePlace : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					instance().chosenPosition_ = TilePositions::Unknown;
					if (!instance().chosenType_.isRefinery())
					{
						if (instance().chosenType_.requiresPsi())
						{
							instance().chosenPosition_ = mapBweb.getBuildPosition(instance().chosenType_, instance().reserved_, Broodwar->self()->getStartLocation(),
								true);
						}
						else
						{
							instance().chosenPosition_ = mapBweb.getBuildPosition(instance().chosenType_, instance().reserved_, Broodwar->self()->getStartLocation(),
								false);
						}
					}
					else
					{
						instance().chosenPosition_ = instance().chooseGeyser();
					}
					if (instance().chosenPosition_ != TilePositions::Unknown)
					{
						return Status::Success;
					}
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};
		class ChooseIdle : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					Unit chosenWorker = nullptr;
					for (auto unit : RecollectManager::instance().workerIdle)
					{
						if (!unit->isIdle())
						{
							continue;
						}
						if (!chosenWorker || unit->getDistance(Position(instance().chosenPosition_)) < chosenWorker->getDistance(
							Position(instance().chosenPosition_)))
						{
							chosenWorker = unit;
						}
					}
					if (chosenWorker)
					{
						RecollectManager::instance().workerIdle.erase(chosenWorker);
						pair<UnitType, TilePosition> placeType = { instance().chosenType_, instance().chosenPosition_ };
						instance().workerBuild.insert(pair<Unit, pair<UnitType, TilePosition>>(chosenWorker, placeType));
						instance().buildingsResourcesQueue.emplace_back(instance().chosenType_);
						if (instance().isFromBO_)
						{
							instance().itemsInProgress_.emplace_back(BuildOrderManager::instance().myBo.itemsBO[0]);
							BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
						}
						chosenWorker->move(Position(instance().chosenPosition_));
						return Status::Success;
					}
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};
		class ChooseMiner : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					Unit chosenWorker = nullptr;
					Unit mineral = nullptr;
					for (const auto unit : RecollectManager::instance().workerMineral)
					{
						if (unit.first->isCarryingMinerals())
						{
							continue;
						}
						if (!chosenWorker || unit.first->getDistance(Position(instance().chosenPosition_)) < chosenWorker->getDistance(
							Position(instance().chosenPosition_)))
						{
							chosenWorker = unit.first;
							mineral = unit.second;
						}
					}
					if (chosenWorker && mineral)
					{
						RecollectManager::instance().workerMineral.erase(chosenWorker);
						ResourceManager::instance().minerals[mineral]--;
						pair<UnitType, TilePosition> placeType = { instance().chosenType_, instance().chosenPosition_ };
						instance().workerBuild.insert(pair<Unit, pair<UnitType, TilePosition>>(chosenWorker, placeType));
						instance().buildingsResourcesQueue.emplace_back(instance().chosenType_);
						if (instance().isFromBO_)
						{
							instance().itemsInProgress_.emplace_back(BuildOrderManager::instance().myBo.itemsBO[0]);
							BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
						}
						chosenWorker->move(Position(instance().chosenPosition_));
						return Status::Success;
					}
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};
		// create a sequence
		auto build = std::make_shared<bt::Sequence>();
		auto chooseWorker = std::make_shared<bt::Selector>();
		const auto chooseTypeBuilding = std::make_shared<ChooseType>();
		const auto choosePlace = std::make_shared<ChoosePlace>();
		const auto chooseIdleWorker = std::make_shared<ChooseIdle>();
		const auto chooseMinerWorker = std::make_shared<ChooseMiner>();
		build->addChild(chooseTypeBuilding);
		build->addChild(choosePlace);
		chooseWorker->addChild(chooseIdleWorker);
		chooseWorker->addChild(chooseMinerWorker);
		build->addChild(chooseWorker);
		// set the root of the tree
		instance().buildingTree_.setRoot(build);
	}
	void BuildingManager::onFrame()
	{
		for (auto& builder : instance().workerBuild)
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
		instance().buildingTree_.update();
	}
}