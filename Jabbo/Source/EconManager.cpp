#include "EconManager.hpp"
#include <BWTA.h>
using namespace BWAPI;
using namespace scutil;
using namespace Filter;
namespace Jabbo {
	EconManager::EconManager()
		= default;

	double EconManager::mineralGainPerFrame() // TODO FIX
	{
		auto rate = 0.0;
		if (Broodwar->getFrameCount() > 0)
		{
			rate = (double(Broodwar->self()->gatheredMinerals() - 50)) / Broodwar->getFrameCount();
		}
		return rate;
	}

	//TODO Real maths
	int EconManager::getMineralsWhenReaching(Unit u, const TilePosition start, const TilePosition end) {
		const auto rate = mineralGainPerFrame();
		const auto distance = BWTA::getGroundDistance(start, end);
		const auto frames = distance / 2.55;
		const auto mineralsWhenReach = static_cast<int>(rate * frames);
		return mineralsWhenReach;
	}

	int EconManager::getMineralsWhenReaching(const TilePosition start, const TilePosition end) {
		const auto rate = mineralGainPerFrame();
		const auto distance = BWTA::getGroundDistance(start, end);
		const auto frames = distance / 2.55;
		const auto mineralsWhenReach = static_cast<int>(rate * frames);
		return mineralsWhenReach;
	}

	EconManager & EconManager::instance()
	{
		static EconManager instance;
		return instance;
	}
}