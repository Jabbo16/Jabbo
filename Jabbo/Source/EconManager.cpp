#include "EconManager.hpp"
#include "BuildingManager.hpp"
#include "UnitInfoManager.h"
#include "MapManager.hpp"
using namespace BWAPI;
using namespace scutil;
using namespace Filter;
namespace Jabbo {
	EconManager::EconManager()
		= default;

	double EconManager::mineralGainPerFrame()
	{
		auto rate = 0.0;
		if (Broodwar->getFrameCount() > 0)
		{
			rate = (double(Broodwar->self()->gatheredMinerals() - 50)) / Broodwar->getFrameCount();
		}
		return rate;
	}

	//TODO Real maths
	int EconManager::getMineralsWhenReaching(Unit u, const Position start, const Position end) {
		const auto rate = mineralGainPerFrame();
		const auto distance = MapManager::getGroundDistance(start, end);
		const auto frames = distance / 2.55;
		const auto mineralsWhenReach = static_cast<int>(rate * frames);
		return mineralsWhenReach;
	}

	int EconManager::getMineralsWhenReaching(const Position start, const Position end) {
		const auto rate = mineralGainPerFrame();
		const auto distance = MapManager::getGroundDistance(start, end);
		const auto frames = distance / 2.55;
		const auto mineralsWhenReach = static_cast<int>(rate * frames);
		return mineralsWhenReach;
	}

	bool EconManager::iHaveMoney(const UnitType& unit)
	{
		auto myMinerals = Broodwar->self()->minerals();
		auto myGas = Broodwar->self()->gas();
		for (auto queue : BuildingManager::instance().buildingsResourcesQueue)
		{
			myMinerals -= queue.mineralPrice();
			myGas -= queue.gasPrice();
		}
		return myMinerals >= unit.mineralPrice() && myGas >= unit.gasPrice();
	}

	bool EconManager::iHaveMoney(const UnitType& unit, const TilePosition start, const TilePosition end) const
	{
		auto myMinerals = Broodwar->self()->minerals();
		auto myGas = Broodwar->self()->gas();
		for (auto queue : BuildingManager::instance().buildingsResourcesQueue)
		{
			myMinerals -= queue.mineralPrice();
			myGas -= queue.gasPrice();
		}
		return myMinerals + getMineralsWhenReaching(Position(start), Position(end)) >= unit.mineralPrice() && myGas >= unit.gasPrice();
	}

	EconManager & EconManager::instance()
	{
		static EconManager instance;
		return instance;
	}
}