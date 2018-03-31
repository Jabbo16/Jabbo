#include "UnitManager.hpp"
#include <UnitInfoManager.h>
using namespace BWAPI;
using namespace scutil;
using namespace Filter;
namespace Jabbo {
	UnitManager::UnitManager()
		= default;

	int UnitManager::getNumberUnits(const UnitType& type)
	{
		return UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self()).getNumUnits(type);
	}

	UnitManager & UnitManager::instance()
	{
		static UnitManager instance;
		return instance;
	}
}