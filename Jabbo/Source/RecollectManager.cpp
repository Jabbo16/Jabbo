#include "RecollectManager.hpp"
#include "ResourceManager.hpp"

namespace Jabbo {
	RecollectManager::RecollectManager()
		= default;

	RecollectManager & RecollectManager::instance()
	{
		static RecollectManager instance;
		return instance;
	}

	void RecollectManager::initTree()
	{
		class ChooseWorker : public BT::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					for (auto unit : instance().workerIdle)
					{
						if (unit->getLastCommandFrame() != Broodwar->getFrameCount())
						{
							instance().chosenWorker = unit;
							return Status::Success;
						}
					}
					instance().chosenWorker = nullptr;
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};

		class GatherMineral : public BT::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					if (instance().chosenWorker != nullptr)
					{
						if (instance().chosenWorker->exists())
						{
							if (ResourceManager::instance().minerals.empty())
							{
								return Status::Failure;
							}
							Unit closestMineral = nullptr;
							for (const auto mineral : ResourceManager::instance().minerals)
							{
								if (mineral.second >= 2)
								{
									continue;
								}
								if (!closestMineral || mineral.first->getDistance(instance().chosenWorker) < closestMineral->getDistance(instance().chosenWorker))
								{
									closestMineral = mineral.first;
								}
							}
							if (closestMineral)
							{
								ResourceManager::instance().minerals[closestMineral]++;
								instance().workerMineral.emplace(pair<Unit, Unit>{instance().chosenWorker, closestMineral});
								instance().workerIdle.erase(instance().chosenWorker);
								instance().chosenWorker->gather(closestMineral);
								instance().chosenWorker = nullptr;
								return Status::Success;
							}
						}
					}
					instance().chosenWorker = nullptr;
					return Status::Failure;
				}
				catch (int e)
				{
					Broodwar->sendText(std::to_string(e).c_str());
					return Status::Invalid;
				}
			}
		};

		class GatherGas : public BT::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					if (instance().chosenWorker != nullptr)
					{
						if (instance().chosenWorker->exists())
						{
							if (ResourceManager::instance().gas.empty())
							{
								return Status::Failure;
							}
							Unit chosenRefinery = nullptr;
							for (const auto gas : ResourceManager::instance().gas)
							{
								if (gas.second >= 3)
								{
									continue;
								}

								chosenRefinery = gas.first;
							}
							if (chosenRefinery)
							{
								ResourceManager::instance().gas[chosenRefinery]++;
								instance().workerGas.emplace(pair<Unit, Unit>{instance().chosenWorker, chosenRefinery});
								instance().workerIdle.erase(instance().chosenWorker);
								instance().chosenWorker->gather(chosenRefinery);
								instance().chosenWorker = nullptr;
								return Status::Success;
							}
						}
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
		auto gather = std::make_shared<BT::Sequence>();
		auto chooseMineralOrGeyser = std::make_shared<BT::Selector>();
		const auto chooseWorkerGather = std::make_shared<ChooseWorker>();
		const auto gatherGeyser = std::make_shared<GatherGas>();
		const auto gatherMinerals = std::make_shared<GatherMineral>();
		gather->addChild(chooseWorkerGather);
		chooseMineralOrGeyser->addChild(gatherGeyser);
		chooseMineralOrGeyser->addChild(gatherMinerals);
		gather->addChild(chooseMineralOrGeyser);
		// set the root of the tree
		instance().recollectionTree_.setRoot(gather);
	}

	void RecollectManager::mineralLocking()
	{
		for (const auto m : instance().workerMineral)
		{
			if (m.first->getLastCommandFrame() == Broodwar->getFrameCount()) continue;
			if (m.first->isIdle()) m.first->gather(m.second);
			if (!m.first->getOrderTarget()) continue;
			if (m.first->getOrderTarget()->getID() != m.second->getID() && !m.first->isCarryingMinerals()) m.first->gather(m.second);
		}
	}
	void RecollectManager::onFrame()
	{
		instance().recollectionTree_.update();
		instance().mineralLocking();
	}

	// Only worker units
	void RecollectManager::onUnitDestroy(const Unit unit)
	{
		// Workers gathering gas
		auto found = instance().workerGas.find(unit);
		auto isIn = found != instance().workerGas.end();
		if (isIn)
		{
			ResourceManager::instance().gas[instance().workerGas[unit]]--;
			instance().workerGas.erase(unit);
			return;
		}

		// Workers gathering minerals
		found = instance().workerMineral.find(unit);
		isIn = found != instance().workerMineral.end();
		if (isIn)
		{
			ResourceManager::instance().minerals[instance().workerMineral[unit]]--;
			instance().workerMineral.erase(found);
			return;
		}

		// Workers being idle
		const auto foundIdle = instance().workerIdle.find(unit);
		isIn = foundIdle != instance().workerIdle.end();
		if (isIn)
		{
			instance().workerIdle.erase(unit);
		}
	}

	void RecollectManager::onUnitRenegade(const Unit unit)
	{
		onUnitDestroy(unit);
	}

	void RecollectManager::onUnitComplete(const Unit unit)
	{
		instance().workerIdle.insert(unit);
	}
}