#pragma once

#include <BWAPI.h>
#include <set>
#include "../BaseManager.hpp"
#include <optional>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	enum workerScoutTasks
	{
		Scouting, Harassing
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
		mutable optional<Base> currentObjective{};
		mutable optional<Base> baseToHarass{};
		mutable bool enemyBaseFound = false;
		mutable workerScoutTasks task{ Scouting };
		mutable Unit unitToHarass = nullptr;
		Unit myUnit = nullptr;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		WorkerScouterAgent(const Unit& unit) : myUnit(unit) {}
		static set<Base> initScouter(bool allBases);
		explicit WorkerScouterAgent(Unit unit, bool allBases);
		WorkerScouterAgent() = default;
		void run() const;
		Unit enemyWorkerToHarass() const;
	};
}