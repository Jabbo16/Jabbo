#include "SimulationManager.hpp"
#include "FAP/FAP.h"
using namespace BWAPI;
using namespace Neolib;
using namespace scutil;
namespace Jabbo {
	SimulationManager::SimulationManager()
		= default;

	bool SimulationManager::simulateHarass(set<UnitInfo>& enemies, const UnitInfo scouter, const int frames)
	{
		fap.clear();
		fap.addUnitPlayer1(FastAPproximation::FAPUnit(scouter));
		for (const auto enemy : enemies)
		{
			fap.addUnitPlayer2(FastAPproximation::FAPUnit(enemy));
		}
		const int preSimFriendlyUnitCount = fap.getState().first->size();
		fap.simulate(frames);
		const int postSimFriendlyUnitCount = fap.getState().first->size();
		const auto myLosses = preSimFriendlyUnitCount - postSimFriendlyUnitCount;
		return myLosses == 0;
	}

	SimulationManager & SimulationManager::instance()
	{
		static SimulationManager instance;
		return instance;
	}
}