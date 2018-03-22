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
		bt::BehaviorTree recollectionTree_;
	public:
		// Left->Worker , Right->Resource
		map<Unit, Unit> workerMineral;
		map<Unit, Unit> workerGas;
		set<Unit> workerIdle;
		Unit chosenWorker = nullptr;
		RecollectManager();
		static void initTree();
		static void mineralLocking();
		static void onFrame();
		static void onUnitDestroy(Unit);
		static void onUnitRenegade(Unit);
		static void onUnitComplete(Unit);
		static RecollectManager & instance();
	};
}