#include "BuildingManager.hpp"
#include "RecollectManager.hpp"
#include "BuildOrderManager.hpp"
#include "BaseManager.hpp"
#include "ResourceManager.hpp"
#include "TrainingManager.hpp"
#include "UnitManager.hpp"

namespace { auto& mapBweb = BWEB::Map::Instance(); }

namespace Jabbo
{
	BuildingManager::BuildingManager() = default;

	bool BuildingManager::iHaveMoney(const UnitType& unit)
	{
		auto myMinerals = Broodwar->self()->minerals();
		auto myGas = Broodwar->self()->gas();
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
			if (base.owner.player != Broodwar->self() || base.geysers.empty())
			{
				continue;
			}
			for (const auto g : base.geysers)
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
			if (builder.second.type == unit->getType() && builder.second.pos == unit->getTilePosition())
			{
				if (Broodwar->self()->getRace() == Races::Terran)
				{
					instance().workerTask.emplace(pair<Unit, Unit>{unit, builder.first});
				}
				else
				{
					instance().unitTransforming_.emplace(pair<Unit, buildInfo>(unit, builder.second));
					RecollectManager::instance().workerIdle.emplace(builder.first);
				}
				for (const auto type : instance().buildingsResourcesQueue)
				{
					if (unit->getType() == type)
					{
						instance().buildingsResourcesQueue.remove(type);
						break;
					}
				}
				if (builder.second.isFromBO)
				{
					instance().itemsInProgress_.emplace(pair<Unit, BOItem>(unit, instance().itemsInProgress_[builder.first]));
					instance().itemsInProgress_.erase(builder.first);
				}
				instance().reserved_.erase(builder.second.pos);
				instance().workerBuild.erase(builder.first);
				break;
			}
		}
	}

	void BuildingManager::onUnitComplete(const Unit unit)
	{
		if (unit->getType().isRefinery())
		{
			ResourceManager::instance().gas.emplace(std::pair<Unit, int>(unit, 0));
		}
		if (Broodwar->self()->getRace() == Races::Terran)
		{
			if (instance().workerTask.find(unit) != instance().workerTask.end())
			{
				RecollectManager::instance().workerIdle.emplace(instance().workerTask[unit]);
				// TODO add to buildings list Terran
				if (instance().itemsInProgress_.find(unit) != instance().itemsInProgress_.end())
				{
					instance().itemsInProgress_.erase(unit);
				}
				instance().workerTask.erase(unit);
			}
		}
		else
		{
			if (instance().unitTransforming_.find(unit) != instance().unitTransforming_.end())
			{
				// TODO add to buildings list Zerg/Protoss
				if (instance().unitTransforming_[unit].isFromBO)
				{
					instance().itemsInProgress_.erase(unit);
				}
				instance().unitTransforming_.erase(unit);
			}
		}
	}

	void BuildingManager::onUnitDestroy(const Unit unit)
	{
		// If unit is my worker
		if (unit->getType().isWorker())
		{
			// If worker is in workerBuild
			if (instance().workerBuild.find(unit) != instance().workerBuild.end())
			{
				const auto aux = instance().workerBuild[unit];

				// If item is from BO, remove from ItemsInProgress and add to myBO
				if (aux.isFromBO)
				{
					if (instance().itemsInProgress_.find(unit) != instance().itemsInProgress_.end())
					{
						BuildOrderManager::instance().myBo.itemsBO.emplace(BuildOrderManager::instance().myBo.itemsBO.begin(), instance().itemsInProgress_[unit]);
						instance().itemsInProgress_.erase(unit);
					}
				}
				// Remove reserved Tile and remove from reserved money buildings list
				instance().reserved_.erase(aux.pos);
				for (auto it = instance().buildingsResourcesQueue.begin(); it != instance().buildingsResourcesQueue.end(); ++it)
				{
					if (*it == aux.type)
					{
						instance().buildingsResourcesQueue.erase(it);
						break;
					}
				}
				instance().workerBuild.erase(unit);
				return;
			}
			// If worker is in workerTask
			for (const auto task : instance().workerTask)
			{
				if (task.second == unit)
				{
					// If item is from BO, remove from ItemsInProgress and add to myBO
					if (instance().itemsInProgress_.find(unit) != instance().itemsInProgress_.end())
					{
						BuildOrderManager::instance().myBo.itemsBO.emplace(BuildOrderManager::instance().myBo.itemsBO.begin(), instance().itemsInProgress_[unit]);
						instance().itemsInProgress_.erase(unit);
					}
					instance().workerTask.erase(unit);
					return;
				}
			}
		}
		// Building died
		else
		{
			// If unit dies while morphing/warping
			if (instance().unitTransforming_.find(unit) != instance().unitTransforming_.end())
			{
				const auto aux = instance().unitTransforming_[unit];
				if (aux.isFromBO)
				{
					// If item is from BO, remove from ItemsInProgress and add to myBO
					if (instance().itemsInProgress_.find(unit) != instance().itemsInProgress_.end())
					{
						BuildOrderManager::instance().myBo.itemsBO.emplace(BuildOrderManager::instance().myBo.itemsBO.begin(), instance().itemsInProgress_[unit]);
						instance().itemsInProgress_.erase(unit);
					}
				}
				instance().unitTransforming_.erase(unit);
				return;
			}

			// We check if the unit being built was in workerTask
			if (instance().workerTask.find(unit) != instance().workerTask.end())
			{
				// If item is from BO, remove from ItemsInProgress and add to myBO
				if (instance().itemsInProgress_.find(unit) != instance().itemsInProgress_.end())
				{
					BuildOrderManager::instance().myBo.itemsBO.emplace(BuildOrderManager::instance().myBo.itemsBO.begin(), instance().itemsInProgress_[unit]);
					instance().itemsInProgress_.erase(unit);
				}
				instance().workerTask.erase(unit);
			}
		}
	}

	void BuildingManager::initTree()
	{
		class ChooseType : public BT::Leaf
		{
		public:
			static bool foundSupplyBeingBuilt()
			{
				switch (Broodwar->self()->getRace())
				{
				case Races::Zerg:
					for (const auto item : TrainingManager::instance().unitQueue)
					{
						if (item.type == Broodwar->self()->getRace().getSupplyProvider())
						{
							return true;
						}
					}
				default:
					for (const auto item : instance().itemsInProgress_)
					{
						if (item.second.unit == Broodwar->self()->getRace().getSupplyProvider())
						{
							return true;
						}
					}
				}
				return false;
			};

			Status update() override
			{
				try
				{
					instance().chosenType_ = UnitTypes::Unknown;
					instance().isFromBO_ = false;
					if (!BuildOrderManager::instance().myBo.itemsBO.empty())
					{
						auto next = BuildOrderManager::instance().myBo.itemsBO[0];

						if (!next.unit.isBuilding())
						{
							if ((Broodwar->self()->supplyTotal() / 2 - Broodwar->self()->supplyUsed() / 2 > 2 && Broodwar->self()->getRace()) || foundSupplyBeingBuilt())
							{
								return Status::Failure;
							}
							const BOItem newItem = { Broodwar->self()->supplyUsed() / 2, Broodwar->self()->getRace().getSupplyProvider() };
							BuildOrderManager::instance().myBo.itemsBO.emplace(BuildOrderManager::instance().myBo.itemsBO.begin(), newItem);
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
						// Supply no Zerg
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
									if (transforming.first->getType() == Broodwar->self()->getRace().getSupplyProvider()) return Status::Failure;
								}
							}
							instance().chosenType_ = Broodwar->self()->getRace().getSupplyProvider();
						}
						else
						{
							switch (Broodwar->self()->getRace())
							{
							case Races::Protoss:
								// Gateways
								if (UnitManager::getNumberUnits(UnitTypes::Protoss_Gateway) < BaseManager::getFriendlyBasesCount(true) * 3 && iHaveMoney(UnitTypes::Protoss_Gateway))
								{
									instance().chosenType_ = UnitTypes::Protoss_Gateway;
									break;
								}
								// TODO Add more different buildings based on EnemyInfo and wanted units to train
							case Races::Terran:
								// Barracks
								if (UnitManager::getNumberUnits(UnitTypes::Terran_Barracks) < BaseManager::getFriendlyBasesCount(true) * 3 && iHaveMoney(UnitTypes::Terran_Barracks))
								{
									instance().chosenType_ = UnitTypes::Terran_Barracks;
									break;
								}
								// TODO Add more different buildings based on EnemyInfo and wanted units to train
							// TODO Zerg logic
							}
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
		class ChoosePlace : public BT::Leaf
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
		class ChooseIdle : public BT::Leaf
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
						buildInfo placeType = { instance().chosenType_, instance().chosenPosition_ , false };
						if (instance().isFromBO_)
						{
							placeType.isFromBO = true;
							instance().itemsInProgress_.emplace(pair<Unit, BOItem>(chosenWorker, BuildOrderManager::instance().myBo.itemsBO[0]));
							BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
						}
						instance().reserved_.emplace(instance().chosenPosition_);
						instance().workerBuild.emplace(pair<Unit, buildInfo>(chosenWorker, placeType));
						instance().buildingsResourcesQueue.emplace_back(instance().chosenType_);
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
		class ChooseMiner : public BT::Leaf
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
						buildInfo placeType = { instance().chosenType_, instance().chosenPosition_ ,false };

						if (instance().isFromBO_)
						{
							placeType.isFromBO = true;
							instance().itemsInProgress_.emplace(pair<Unit, BOItem>(chosenWorker, BuildOrderManager::instance().myBo.itemsBO[0]));
							BuildOrderManager::instance().myBo.itemsBO.erase(BuildOrderManager::instance().myBo.itemsBO.begin());
						}
						instance().reserved_.emplace(instance().chosenPosition_);
						instance().workerBuild.emplace(pair<Unit, buildInfo>(chosenWorker, placeType));
						instance().buildingsResourcesQueue.emplace_back(instance().chosenType_);
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
		auto build = std::make_shared<BT::Sequence>();
		auto chooseWorker = std::make_shared<BT::Selector>();
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
			if (Broodwar->isVisible(builder.second.pos))
			{
				builder.first->build(builder.second.type, builder.second.pos);
			}
		}
		instance().buildingTree_.update();
	}
}