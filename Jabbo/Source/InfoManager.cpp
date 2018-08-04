#include "InfoManager.hpp"
#include "BWEB/BWEB.h"

using namespace BWAPI;
namespace { auto& mapBweb = BWEB::Map::Instance(); }
namespace Jabbo {
	Base InfoManager::getBaseFromTile(const TilePosition tile)
	{
		for (const auto &base : BaseManager::instance().bases)
		{
			if (base.tile == tile)
			{
				return base;
			}
		}
		return {};
	}

	void InfoManager::onStart()
	{
		instance().myRace = Broodwar->self()->getRace();
		instance().mainChoke.emplace(mapBweb.getMainChoke());
		instance().naturalChoke.emplace(mapBweb.getNaturalChoke());
		instance().mainNaturalBase.emplace(getBaseFromTile(mapBweb.getNaturalTile()));
	}

	InfoManager::InfoManager()
		= default;

	scutil::UIMap InfoManager::getUnits(const Player player)
	{
		return scutil::UnitInfoManager::getInstance().getUnitDataOfPlayer(player).getUnits();
	}

	scutil::UIMap InfoManager::getUnits()
	{
		return scutil::UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self()).getUnits();
	}

	InfoManager & InfoManager::instance()
	{
		static InfoManager instance;
		return instance;
	}
}