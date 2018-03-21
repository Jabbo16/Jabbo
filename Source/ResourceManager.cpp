#include "ResourceManager.hpp"
namespace { auto & bwem = BWEM::Map::Instance(); }
using namespace BWEM;
using namespace BWAPI;
namespace Jabbo {
	ResourceManager::ResourceManager()
		= default;

	ResourceManager & ResourceManager::Instance()
	{
		static ResourceManager instance;
		return instance;
	}
	void ResourceManager::onUnitDestroy(const BWAPI::Unit unit)
	{
	}

	void ResourceManager::initBaseResources(const Unit &firstDepot)
	{
		const auto tStart = Broodwar->self()->getStartLocation();
		auto pStart = Position(tStart) + Position(64, 48);
		const auto mainArea = Map::Instance().GetArea(tStart);
		for (auto &base : mainArea->Bases())
		{
			if (base.Location() == firstDepot->getTilePosition())
			{
				for (auto &m : base.Minerals())
				{
					Instance().minerals_.insert(std::pair<Unit, int>(m->Unit(), 0));
				}
				for (auto &g : base.Geysers())
				{
					Instance().gas_.insert(std::pair<Unit, int>(g->Unit(), 0));
				}
			}
		}
	}
}