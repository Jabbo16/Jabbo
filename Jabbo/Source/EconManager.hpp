#pragma once
#include <BWAPI.h>
#include "BaseManager.hpp"
#include "UnitInfoManager.h"
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class EconManager
	{
	public:
		EconManager();
		static double mineralGainPerFrame();
		static int getMineralsWhenReaching(Unit u, TilePosition start, TilePosition end);
		static int getMineralsWhenReaching(const TilePosition start, const TilePosition end);
		static EconManager & instance();
	};
}