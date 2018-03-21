#pragma once

#include <BWAPI.h>
#include <set>
#include "BrainTree.h"
namespace bt = BrainTree;

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	class RecollectManager
	{
	public:
		bt::BehaviorTree RecollectionTree;
		map<Unit, Unit> workerMineral_;
		map<Unit, Unit> workerGas_;
		set<Unit> workerIdle_;
		Unit chosenWorker = nullptr;
		RecollectManager();
		static void init();
		static void mineralLocking();
		static void onFrame();
		static void onUnitDestroy(BWAPI::Unit unit);
		static void onUnitRenegade(BWAPI::Unit unit);
		static void onUnitComplete(BWAPI::Unit unit);
		static RecollectManager & Instance();
	};
}