#include "WallManager.hpp"
#include "BWEB/BWEB.h"
#include "BuildOrderManager.hpp"

using namespace BWAPI;
namespace { auto & mapBweb = BWEB::Map::Instance(); }

namespace Jabbo {
	WallManager::WallManager()
		= default;

	void WallManager::onStart()
	{
		const auto choke = BuildOrderManager::instance().myBo.fe ? mapBweb.getNaturalChoke() : mapBweb.getMainChoke();
		const auto area = BuildOrderManager::instance().myBo.fe ? mapBweb.getNaturalArea() : mapBweb.getMainArea();
		mapBweb.createWall(wallT2, area, choke);
		mainWall_ = *mapBweb.getWalls().begin();
	}

	WallManager & WallManager::instance()
	{
		static WallManager instance;
		return instance;
	}

	BWEB::Wall WallManager::getMainWall() const
	{
		return mainWall_;
	}
}