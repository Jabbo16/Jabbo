#include "WorkerScouterAgent.hpp"
#include "../InfoManager.hpp"
#include "../SimulationManager.hpp"
#include "../MapManager.hpp"

namespace { auto & bwem = BWEM::Map::Instance(); }
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

	void WorkerScouterAgent::getNewHarassTask() const
	{
		// Checking atackers
		this->attackers.clear();
		for (auto u : InfoManager::getUnits(this->baseToHarass->owner.player)) {
			if (u.second.type.isBuilding() || !u.second.type.canAttack())
			{
				continue;
			}

			const auto target = u.first->getTarget() == nullptr ? u.first->getOrderTarget() : u.first->getTarget();
			if (target && target == this->myUnit) {
				this->attackers.emplace(u.second);
			}
		}
		if (this->attackers.empty())
		{
			if (bwem.GetNearestArea(WalkPosition(this->myUnit->getPosition())) != bwem.GetNearestArea(WalkPosition(this->baseToHarass->tile)))
			{
				this->harassStatus = None;
				return;
			}
			if (!Broodwar->isVisible(this->baseToHarass->tile))
			{
				this->harassStatus = None;
				return;
			}
			this->harassStatus = AttackingWorker;
			return;
		}
		const auto win = SimulationManager::simulateHarass(this->attackers, this->myUnit, 85);
		if (win)
		{
			this->harassStatus = AttackingWorker;
			return;
		}
		this->harassStatus = Fleeing;
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
			getNewHarassTask();
			switch (harassStatus)
			{
			case None:
				myUnit->move(Position(baseToHarass->tile));
				fleePosition = Positions::None;
				unitToHarass = nullptr;

			case Fleeing:
			{
				const auto newFleePosition = MapManager::getKitePosition(attackers, myUnit);
				if (newFleePosition == Positions::None)
				{
					fleePosition = Positions::None;
					myUnit->move(Position(baseToHarass->tile));
					unitToHarass = nullptr;
				}
				else if (fleePosition != newFleePosition)
				{
					fleePosition = newFleePosition;
					myUnit->move(fleePosition);
					unitToHarass = nullptr;
				}
				break;
			}
			case AttackingWorker:
				if (!unitToHarass || !unitToHarass->exists())
				{
					const auto workerToAttack = enemyWorkerToHarass();
					if (workerToAttack)
					{
						if (unitToHarass != workerToAttack)
						{
							myUnit->attack(workerToAttack);
							unitToHarass = workerToAttack;
						}
					}
				}
				break;
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
			auto lowLife = 99999;    // ignore any beyond this range
			auto const maxDist = 500;
			// Failing that, find the enemy worker with lowest hp left

			for (const auto unit : InfoManager::getUnits(baseToHarass->owner.player))
			{
				if (unit.second.type.isWorker())
				{
					if (unit.first->getDistance(Position(baseToHarass->tile)) > maxDist)
					{
						continue;
					}
					const auto life = unit.second.lastHealth + unit.second.lastShields;

					if (life < lowLife)
					{
						lowLife = life;
						enemyWorker = unit.first;
					}
				}
			}
			return enemyWorker;
		}
		return nullptr;
	}

	// Choose an enemy worker to harass, or none.
	Unit WorkerScouterAgent::getClosestThreat() const
	{
		Unit threat = nullptr;
		auto dist = 99999;
		// Failing that, find the enemy worker with lowest hp left

		for (const auto unit : InfoManager::getUnits(baseToHarass->owner.player))
		{
			const auto newDist = myUnit->getDistance(unit.second.lastPosition);
			if (!threat || newDist < dist)
			{
				dist = newDist;
				threat = unit.first;
			}
		}
		return threat;
	}
}