#pragma once

#include <BWAPI.h>
#include <set>
#include "Agents/WorkerScouterAgent.hpp"
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	class ScoutingManager
	{
	public:
		ScoutingManager() = default;
		bool initialScoutingDone = false;
		set<WorkerScouterAgent> workerScouters;
		static void onFrame();
		static void onUnitDestroy(Unit unit);
		static void firstScouting();
		static ScoutingManager & instance();
		static pair<Unit, Unit> chooseWorker();
	};
}