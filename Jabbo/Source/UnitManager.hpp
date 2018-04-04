#pragma once
#include <BWAPI.h>
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