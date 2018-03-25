#pragma once
#include <BWAPI.h>
#include "BaseManager.hpp"
#include "UnitInfoManager.h"
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class UnitManager
	{
	public:
		UnitManager();
		static int getNumberUnits(const UnitType& type);
		static UnitManager & instance();
	};
}