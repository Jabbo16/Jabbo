#include "BaseManager.hpp"
#include "InfoManager.hpp"

namespace { auto & bwem = BWEM::Map::Instance(); }
namespace Jabbo
{
	BaseManager::BaseManager()
		= default;

	int BaseManager::getFriendlyBasesCount(const bool mineOnly)
	{
		auto cont = 0;
		for (const auto &base : instance().bases)
		{
			if (mineOnly && base.owner.player != Broodwar->self())
			{
				continue;
			}
			if (base.owner.team == Ally)
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
				newBase.tile = base.Location();
				if (base.Starting())
				{
					newBase.starting = true;
				}
				if (base.Location() == Broodwar->self()->getStartLocation())
				{
					newBase.owner = { Ally, Broodwar->self() }; // TODO add players properly (allied)
					InfoManager::instance().mainBase.emplace(newBase);
				}
				else
				{
					newBase.owner = { Neutral, Broodwar->neutral() };
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