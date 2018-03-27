#include "ScoutingManager.hpp"
#include "RecollectManager.hpp"
#include "ResourceManager.hpp"

namespace Jabbo
{
	void ScoutingManager::onFrame()
	{
		for (auto &scouter : instance().workerScouters)
		{
			scouter.run();
			if (scouter.basesToExplore.empty() && !scouter.currentObjective)
			{
				onUnitDestroy(scouter.myUnit);
				RecollectManager::instance().workerIdle.insert(scouter.myUnit);
			}
		}
	}

	void ScoutingManager::onUnitDestroy(const Unit unit)
	{
		if (instance().workerScouters.find(unit) != instance().workerScouters.end())
		{
			instance().workerScouters.erase(unit);
		}
	}

	void ScoutingManager::firstScouting()
	{
		const auto scouter = chooseWorker();
		if (!scouter.first && !scouter.second)
		{
			return;
		}
		if (scouter.first)
		{
			if (RecollectManager::instance().workerIdle.find(scouter.first) != RecollectManager::instance().workerIdle.end())
			{
				RecollectManager::instance().workerIdle.erase(scouter.first);
				instance().workerScouters.emplace(WorkerScouterAgent{ scouter.first, false });
				instance().initialScoutingDone = true;
				return;
			}
			if (scouter.second)
			{
				if (RecollectManager::instance().workerMineral.find(scouter.first) != RecollectManager::instance().workerMineral.end())
				{
					RecollectManager::instance().workerMineral.erase(scouter.first);
				}
				if (ResourceManager::instance().minerals.find(scouter.second) != ResourceManager::instance().minerals.end())
				{
					ResourceManager::instance().minerals[scouter.second]--;
				}
			}
		}
		instance().workerScouters.emplace(WorkerScouterAgent{ scouter.first, false });
		instance().initialScoutingDone = true;
	}
	ScoutingManager & ScoutingManager::instance()
	{
		static ScoutingManager instance;
		return instance;
	}

	pair<Unit, Unit> ScoutingManager::chooseWorker()
	{
		for (auto unit : RecollectManager::instance().workerIdle)
		{
			if (unit->getLastCommandFrame() != Broodwar->getFrameCount())
			{
				return pair<Unit, Unit>(unit, nullptr);
			}
		}
		for (const auto unit : RecollectManager::instance().workerMineral)
		{
			if (unit.first->isCarryingMinerals())
			{
				continue;
			}
			if (unit.first->getLastCommandFrame() != Broodwar->getFrameCount())
			{
				return unit;
			}
		}
		return pair<Unit, Unit>(nullptr, nullptr);
	}
}