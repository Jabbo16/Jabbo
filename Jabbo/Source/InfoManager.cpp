#include "InfoManager.hpp"
#include "BWEB/BWEB.h"
#include <UnitInfoManager.h>

using namespace BWAPI;
namespace { auto & bwem = BWEM::Map::Instance(); }
namespace { auto& mapBweb = BWEB::Map::Instance(); }
namespace Jabbo {
	Base InfoManager::getBaseFromTile(const TilePosition tile)
	{
		for (auto &base : BaseManager::instance().bases)
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
		instance().mainChoke.emplace(mapBweb.getFirstChoke());
		instance().naturalChoke.emplace(mapBweb.getSecondChoke());
		instance().mainNaturalBase.emplace(getBaseFromTile(mapBweb.getNatural()));
	}

	InfoManager::InfoManager()
		= default;

	scutil::UIMap InfoManager::getUnits(const Player player)
	{
		return scutil::UnitInfoManager::getInstance().getUnitDataOfPlayer(player).getUnits();
	}

	InfoManager & InfoManager::instance()
	{
		static InfoManager instance;
		return instance;
	}
}