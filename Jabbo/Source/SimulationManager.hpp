#pragma once
#include <BWAPI.h>
#include <UnitInfoManager.h>
#include <set>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class SimulationManager
	{
	public:
		SimulationManager();
		static bool simulateHarass(set<scutil::UnitInfo> &enemies, scutil::UnitInfo scouter, int frames);
		static SimulationManager & instance();
	};
}