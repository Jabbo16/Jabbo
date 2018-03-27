#include "BaseManager.hpp"

namespace Jabbo
{
	BaseManager::BaseManager()
		= default;

	int BaseManager::getFriendlyBasesCount()
	{
		auto cont = 0;
		for (const auto &base : instance().bases)
		{
			if (base.owner == Ally)
			{
				cont++;
			}
		}
		return cont;
	}

	void BaseManager::initBaseManager()
	{
		for (const auto &area : bwem.Areas())
		{
			for (const auto &base : area.Bases())
			{
				if (base.Minerals().empty())
				{
					continue;
				}
				Base newBase;
				for (auto& m : base.Minerals())
				{
					newBase.minerals.insert(m->Unit());
				}
				for (auto& g : base.Geysers())
				{
					newBase.geysers.insert(pair<Unit, bool>(g->Unit(), false));
				}
				newBase.area = base.GetArea();
				newBase.base = &base;
				if (base.Location() == Broodwar->self()->getStartLocation())
				{
					newBase.owner = Ally;
				}
				else
				{
					newBase.owner = Neutral;
				}
				newBase.tile = base.Location();
				if (base.Starting())
				{
					newBase.starting = true;
				}
				instance().bases.emplace(newBase);
			}
		}
		Broodwar->sendText("Found %d bases", instance().bases.size());
	}

	BaseManager& BaseManager::instance()
	{
		static BaseManager instance;
		return instance;
	}
}