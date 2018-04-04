#pragma once
#include <BWAPI.h>
#include "BaseManager.hpp"
#include"BWEM 1.4.1/src/bwem.h"
#include <optional>
#include <UnitInfoManager.h>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class InfoManager
	{
	public:
		optional<Base> mainBase;
		optional<Base> mainNaturalBase;
		optional<const ChokePoint*> mainChoke;
		optional<const ChokePoint*> naturalChoke;
		Race myRace = Races::Unknown;
		static Base getBaseFromTile(TilePosition tile);
		static void onStart();
		InfoManager();
		static scutil::UIMap getUnits(Player player);
		static InfoManager & instance();
	};
}