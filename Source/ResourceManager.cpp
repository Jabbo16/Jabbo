#include "ResourceManager.hpp"

using namespace BWAPI;
namespace Jabbo {
	ResourceManager::ResourceManager()
		= default;

	ResourceManager & ResourceManager::instance()
	{
		static ResourceManager instance;
		return instance;
	}
	void ResourceManager::onUnitDestroy(const BWAPI::Unit unit)
	{
	}

	void ResourceManager::initBaseResources()
	{
		for (auto &base : BaseManager::instance().bases)
		{
			if (base.owner == Ally)
			{
				for (auto &m : base.minerals)
				{
					instance().minerals.insert(std::pair<Unit, int>(m, 0));
				}
				for (auto &g : base.geysers)
				{
					instance().gas.insert(std::pair<Unit, int>(g, 0));
				}
				break;
			}
		}
	}
}