#pragma once

#include <BWAPI.h>
#include"BWEM 1.4.1/src/bwem.h"
#include <set>
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class ResourceManager
	{
	public:
		map<Unit, int> minerals_;
		map<Unit, int> gas_;
		ResourceManager();
		static void onUnitDestroy(BWAPI::Unit unit);
		static void initBaseResources(const Unit& firstDepot);
		static ResourceManager & Instance();
	};
}