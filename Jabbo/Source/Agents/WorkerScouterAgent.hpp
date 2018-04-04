#pragma once
#include <BWAPI.h>
#include <set>
#include "../BaseManager.hpp"
#include <optional>
#include "../include/UnitData.h"

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	enum workerScoutTasks
	{
		Scouting, Harassing
	};

	enum harassTasks
	{
		AttackingWorker, Fleeing, None
	};
	inline const char* toString(const workerScoutTasks v)
	{
		switch (v)
		{
		case Scouting:   return "Scouting";
		case Harassing:   return "Harassing";
		default:      return "Scouting";
		}
	}
	inline string toString(const harassTasks v)
	{
		switch (v)
		{
		case AttackingWorker:   return "AttackingWorker";
		case Fleeing:   return "Fleeing";
		case None:   return "None";
		default:      return "None";
		}
	}
	class WorkerScouterAgent
	{
	public:
		bool operator<(const WorkerScouterAgent &cScouter) const {
			return myUnit < cScouter.myUnit;
		}
		bool operator==(const WorkerScouterAgent &cScouter) const {
			return myUnit == cScouter.myUnit;
		}
		mutable set<Base> basesToExplore;
		mutable set<scutil::UnitInfo> attackers;
		mutable optional<Base> currentObjective{};
		mutable optional<Base> baseToHarass{};
		mutable bool enemyBaseFound = false;
		mutable workerScoutTasks task{ Scouting };
		mutable harassTasks harassStatus{ None };
		mutable Unit unitToHarass = nullptr;
		Unit myUnit = nullptr;
		mutable Position fleePosition;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		WorkerScouterAgent(const Unit& unit) : myUnit(unit) {}
		static set<Base> initScouter(bool allBases);
		explicit WorkerScouterAgent(Unit unit, bool allBases);
		WorkerScouterAgent() = default;
		void getNewHarassTask() const;
		void run() const;
		Unit enemyWorkerToHarass() const;
		Unit getClosestThreat() const;
	};
}