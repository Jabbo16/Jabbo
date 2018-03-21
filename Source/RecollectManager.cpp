#include "RecollectManager.hpp"
#include "ResourceManager.hpp"

namespace Jabbo {
	RecollectManager::RecollectManager()
		= default;

	RecollectManager & RecollectManager::Instance()
	{
		static RecollectManager instance;
		return instance;
	}

	void RecollectManager::init()
	{
		class ChooseWorker : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					for (auto unit : Instance().workerIdle_)
					{
						if (unit->getLastCommandFrame() != Broodwar->getFrameCount())
						{
							Instance().chosenWorker = unit;
							return Status::Success;
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

		class gatherMineral : public bt::Leaf
		{
		public:
			Status update() override
			{
				try
				{
					if (Instance().chosenWorker != nullptr)
					{
						if (Instance().chosenWorker->exists())
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
								if (!closestMineral || mineral.first->getDistance(Instance().chosenWorker) < closestMineral->getDistance(Instance().chosenWorker))
								{
									closestMineral = mineral.first;
								}
							}
							if (closestMineral)
							{
								ResourceManager::instance().minerals_[closestMineral]++;
								Instance().workerMineral_.insert(pair<Unit, Unit>{Instance().chosenWorker, closestMineral});
								Instance().workerIdle_.erase(Instance().chosenWorker);
								Instance().chosenWorker->gather(closestMineral);
								Instance().chosenWorker = nullptr;
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
		auto gather = std::make_shared<bt::Sequence>();
		const auto chooseWorkerGather = std::make_shared<ChooseWorker>();
		const auto gatherMinerals = std::make_shared<gatherMineral>();
		gather->addChild(chooseWorkerGather);
		gather->addChild(gatherMinerals);
		// set the root of the tree
		Instance().RecollectionTree.setRoot(gather);
	}

	void RecollectManager::mineralLocking()
	{
		for (const auto m : Instance().workerMineral_)
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
		Instance().RecollectionTree.update();
		Instance().mineralLocking();
	}

	void RecollectManager::onUnitDestroy(const BWAPI::Unit unit)
	{
		auto found = Instance().workerGas_.find(unit);
		auto isIn = found != Instance().workerGas_.end();
		if (isIn)
		{
			Instance().workerGas_.erase(found);
		}
		found = Instance().workerMineral_.find(unit);
		isIn = found != Instance().workerMineral_.end();
		if (isIn)
		{
			Instance().workerMineral_.erase(found);
		}
		const auto foundIdle = Instance().workerIdle_.find(unit);
		isIn = foundIdle != Instance().workerIdle_.end();
		if (isIn)
		{
			Instance().workerIdle_.erase(foundIdle);
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
			Instance().workerIdle_.insert(unit);
		}
	}
}