#include "ResourceManager.hpp"
#include "RecollectManager.hpp"
#include <set>
#include "BaseManager.hpp"
using namespace BWAPI;
namespace Jabbo {
	ResourceManager::ResourceManager()
		= default;

	ResourceManager & ResourceManager::instance()
	{
		static ResourceManager instance;
		return instance;
	}
	void ResourceManager::onMineralDestroy(const Unit unit)
	{
		for (auto base : BaseManager::instance().bases)
		{
			if (base.minerals.find(unit) != base.minerals.end())
			{
				base.minerals.erase(unit);
				if (base.owner.player == Broodwar->self())
				{
					instance().minerals.erase(unit);
					for (auto mineral = RecollectManager::instance().workerMineral.begin(); mineral != RecollectManager::instance().workerMineral.end();)
					{
						if (mineral->second == unit)
						{
							RecollectManager::instance().workerIdle.insert(mineral->first);
							mineral->first->stop();
							mineral = RecollectManager::instance().workerMineral.erase(mineral);
						}
						else
						{
							++mineral;
						}
					}
				}
			}
		}
	}

	void ResourceManager::initBaseResources()
	{
		for (const auto& base : BaseManager::instance().bases)
		{
			if (base.owner.player == Broodwar->self())
			{
				for (auto &m : base.minerals)
				{
					instance().minerals.insert(std::pair<Unit, int>(m, 0));
				}
				//				for (auto &g : base.geysers)
				//				{
				//					instance().gas.insert(std::pair<Unit, int>(g.first, 0));
				//				}
				break;
			}
		}
	}
}