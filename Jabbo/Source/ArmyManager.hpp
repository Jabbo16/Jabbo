#pragma once
#include <BWAPI.h>
#include <set>
#include "Squad.hpp"

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class ArmyManager
	{
	private:
		mutable set<Squad> myArmy_{};
		inline static int id_ = 0;
	public:
		void onUnitComplete(scutil::UnitInfo u) const;
		void onUnitDestroy(Unit u) const;
		set<Squad>* getArmy() const;
		static bool isCombatUnit(UnitType type);
		void updateSquads() const;
		void runSquads() const;
		void onFrame();
		void mergeSquads() const;
		ArmyManager();
		static ArmyManager & instance();
	};
}