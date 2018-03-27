#include "WorkerScouterAgent.hpp"
#include "../DrawManager.hpp"

namespace Jabbo
{
	set<Base> WorkerScouterAgent::initScouter(const bool allBases)
	{
		set<Base> aux{};
		for (const auto &base : BaseManager::instance().bases)
		{
			if (base.owner != Neutral || (!allBases && !base.starting) || Broodwar->isVisible(base.tile))
			{
				continue;
			}
			aux.insert(base);
		}
		return aux;
	}

	WorkerScouterAgent::WorkerScouterAgent(const Unit unit, const bool allBases)
	{
		myUnit = unit;
		basesToExplore = initScouter(allBases);
	}

	// TODO Add InfoManager for scouting tasks and related structures like this agent.
	void WorkerScouterAgent::run() const
	{
		if (!currentObjective || !currentObjective.has_value())
		{
			currentObjective.emplace(*basesToExplore.begin());
			myUnit->move(Position(currentObjective->tile));
			basesToExplore.erase(basesToExplore.begin());
		}
		else
		{
			if (Broodwar->isVisible(currentObjective->tile))
			{
				// TODO check enemies near real implementation
				for (auto unit : Broodwar->getUnitsInRadius(myUnit->getPosition(), 500))
				{
					if (unit->getPlayer() != Broodwar->enemy() || !unit->getType().isBuilding())
					{
						continue;
					}
					for (const auto &base : BaseManager::instance().bases)
					{
						if (base.owner == Ally)
						{
							continue;
						}
						if (base.tile == currentObjective->tile)
						{
							currentObjective->owner = Enemy;
							Broodwar->sendText("Enemy Found!");
							BaseManager::instance().bases.erase(base);
							BaseManager::instance().bases.insert(currentObjective.value());
							currentObjective.reset();
							enemyBaseFound = true;
							return;
						}
					}
				}
				currentObjective.reset();
			}
		}
	}
}