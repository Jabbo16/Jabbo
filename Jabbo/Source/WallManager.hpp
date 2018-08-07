#pragma once
#include <BWAPI.h>
#include "BWEB/BWEB.h"

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	inline auto wallT1 = std::vector<UnitType>{ UnitTypes::Terran_Supply_Depot, UnitTypes::Terran_Barracks };
	inline auto wallT2 = std::vector<UnitType>{ UnitTypes::Terran_Supply_Depot, UnitTypes::Terran_Barracks, UnitTypes::Terran_Supply_Depot };
	inline auto wallT3 = std::vector<UnitType>{ UnitTypes::Terran_Supply_Depot, UnitTypes::Terran_Barracks,UnitTypes::Terran_Bunker };
	inline auto wallT4 = std::vector<UnitType>{ UnitTypes::Terran_Supply_Depot, UnitTypes::Terran_Barracks,UnitTypes::Terran_Bunker,UnitTypes::Terran_Supply_Depot };

	class WallManager
	{
		BWEB::Wall mainWall_;
	public:
		WallManager();
		void onStart();
		static WallManager & instance();
		BWEB::Wall getMainWall() const;
	};
}