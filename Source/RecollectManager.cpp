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
		class ChooseWorker : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					for (auto unit : instance().workerIdle_)
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

		class gatherMineral : public bt::Leaf
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
							if (ResourceManager::instance().minerals_.empty())
							{
								return Status::Failure;
							}
							Unit closestMineral = nullptr;
							for (const auto mineral : ResourceManager::instance().minerals_)
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
								ResourceManager::instance().minerals_[closestMineral]++;
								instance().workerMineral_.insert(pair<Unit, Unit>{instance().chosenWorker, closestMineral});
								instance().workerIdle_.erase(instance().chosenWorker);
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

		// create a sequence
		auto gather = std::make_shared<bt::Sequence>();
		const auto chooseWorkerGather = std::make_shared<ChooseWorker>();
		const auto gatherMinerals = std::make_shared<gatherMineral>();
		gather->addChild(chooseWorkerGather);
		gather->addChild(gatherMinerals);
		// set the root of the tree
		instance().RecollectionTree.setRoot(gather);
	}

	void RecollectManager::mineralLocking()
	{
		for (const auto m : instance().workerMineral_)
		{
			if (m.first->getLastCommandFrame() == Broodwar->getFrameCount())
			{
				continue;
			}
			if (!m.first->getOrderTarget())
			{
				continue;
			}
			if (m.first->getOrderTarget()->getID() != m.second->getID() && !m.first->isCarryingMinerals())
			{
				m.first->gather(m.second);
			}
		}
	}
	void RecollectManager::onFrame()
	{
		instance().RecollectionTree.update();
		instance().mineralLocking();
	}

	void RecollectManager::onUnitDestroy(const BWAPI::Unit unit)
	{
		auto found = instance().workerGas_.find(unit);
		auto isIn = found != instance().workerGas_.end();
		if (isIn)
		{
			instance().workerGas_.erase(found);
		}
		found = instance().workerMineral_.find(unit);
		isIn = found != instance().workerMineral_.end();
		if (isIn)
		{
			instance().workerMineral_.erase(found);
		}
		const auto foundIdle = instance().workerIdle_.find(unit);
		isIn = foundIdle != instance().workerIdle_.end();
		if (isIn)
		{
			instance().workerIdle_.erase(foundIdle);
		}
	}

	void RecollectManager::onUnitRenegade(BWAPI::Unit unit)
	{
		onUnitDestroy(unit);
	}

	void RecollectManager::onUnitComplete(const BWAPI::Unit unit)
	{
		if (unit->getType().isWorker())
		{
			instance().workerIdle_.insert(unit);
		}
	}
}