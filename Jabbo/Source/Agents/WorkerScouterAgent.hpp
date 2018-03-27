#pragma once

#include <BWAPI.h>
#include <set>
#include "../BaseManager.hpp"
#include <optional>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

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
		mutable bool enemyBaseFound = false;
		Unit myUnit = nullptr;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		WorkerScouterAgent(const Unit& unit) : myUnit(unit) {}
		static set<Base> initScouter(bool allBases);
		explicit WorkerScouterAgent(Unit unit, bool allBases);
		WorkerScouterAgent() = default;
		void run() const;
	};
}