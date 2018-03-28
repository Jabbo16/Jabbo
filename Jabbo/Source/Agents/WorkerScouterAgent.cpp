#include "WorkerScouterAgent.hpp"
#include "../InfoManager.hpp"

namespace Jabbo
{
	set<Base> WorkerScouterAgent::initScouter(const bool allBases)
	{
		set<Base> aux{};
		for (const auto &base : BaseManager::instance().bases)
		{
			if (base.owner.team != Neutral || (!allBases && !base.starting) || Broodwar->isVisible(base.tile))
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
		task = Scouting;
	}

	void WorkerScouterAgent::run() const
	{
		switch (task)
		{
		case Scouting:
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
							if (base.owner.team == Ally)
							{
								continue;
							}
							if (base.tile == currentObjective->tile)
							{
								currentObjective->owner.team = Enemy;
								currentObjective->owner.player = unit->getPlayer(); // TODO Add player properly and continue scouting if more than one enemy player
								Broodwar->sendText("Enemy Found!");
								BaseManager::instance().bases.erase(base);
								BaseManager::instance().bases.emplace(currentObjective.value());
								baseToHarass.emplace(currentObjective.value());
								currentObjective.reset();
								enemyBaseFound = true;
								task = Harassing;
								return;
							}
						}
					}
					currentObjective.reset();
				}
			}
			break;
		case Harassing:
			if (!unitToHarass || !unitToHarass->exists())
			{
				const auto workerToAttack = enemyWorkerToHarass();
				if (workerToAttack)
				{
					if (unitToHarass != workerToAttack)
					{
						Broodwar->sendText("ATTACK MODE ACTIVATED!");
						myUnit->attack(workerToAttack);
						unitToHarass = workerToAttack;
					}
				}
			}
			break;
		}
	}

	// Choose an enemy worker to harass, or none.
	Unit WorkerScouterAgent::enemyWorkerToHarass() const
	{
		if (baseToHarass)
		{
			// First look for any enemy worker that is building.
			for (const auto unit : InfoManager::getUnits(baseToHarass->owner.player))
			{
				if (unit.second.type.isWorker() && unit.first->isConstructing())
				{
					return unit.first;
				}
			}

			Unit enemyWorker = nullptr;
			auto maxDist = 500;    // ignore any beyond this range

								   // Failing that, find the enemy worker closest to the depot/base tile.

			for (const auto unit : Broodwar->enemy()->getUnits())
			{
				if (unit->getType().isWorker())
				{
					const auto dist = unit->getDistance(Position(baseToHarass->tile));

					if (dist < maxDist)
					{
						maxDist = dist;
						enemyWorker = unit;
					}
				}
			}
			return enemyWorker;
		}
		return nullptr;
	}
}