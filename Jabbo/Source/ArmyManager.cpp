#include "ArmyManager.hpp"
#include "MapManager.hpp"
#include "InfoManager.hpp"

namespace Jabbo
{
	void ArmyManager::onUnitComplete(const scutil::UnitInfo u) const
	{
		if (u.player == Broodwar->self() && isCombatUnit(u.type))
		{
			myArmy_.emplace(Squad(u, id_));
			id_++;
		}
	}

	void ArmyManager::onUnitDestroy(const Unit u) const
	{
		if (isCombatUnit(u->getType()))
		{
			for (auto& s : myArmy_)
			{
				if (s.members.find(u) != s.members.end())
				{
					s.members.erase(u);
					if (s.members.empty())
					{
						myArmy_.erase(s);
					}
					return;
				}
			}
		}
	}

	set<Squad>* ArmyManager::getArmy() const
	{
		return &this->myArmy_;
	}

	bool ArmyManager::isCombatUnit(const UnitType type)
	{
		return type == UnitTypes::Terran_Marine;
	}

	void ArmyManager::updateSquads() const
	{
		for (const auto& s : myArmy_)
		{
			for (const auto& u : s.members)
			{
				s.members[u.first] = InfoManager::getUnits()[u.first];
			}
			s.updateSquadCenter();
		}
	}

	void ArmyManager::runSquads() const
	{
		for (auto& s : myArmy_)
		{
			s.run();
		}
	}

	void ArmyManager::onFrame()
	{
		updateSquads();
		mergeSquads();
		runSquads();
	}

	void ArmyManager::mergeSquads() const
	{
		if (myArmy_.size() < 2) return;
		auto changed = false;
		do
		{
			changed = false;
			for (auto& s1 : myArmy_)
			{
				if (s1.members.empty()) continue;
				for (auto& s2 : myArmy_)
				{
					if (s2.members.empty()) continue;
					if (s1 == s2) continue;

					if (s1.status == s2.status && MapManager::instance().broodWarDistance(s1.center, s2.center) <= 300)
					{
						s1.members.insert(s2.members.begin(), s2.members.end());
						s1.updateSquadCenter();
						s2.members.clear();
						changed = true;
						break;
					}
				}
			}
			auto it = myArmy_.begin();
			while (it != myArmy_.end())
			{
				if (it->members.empty()) it = myArmy_.erase(it);
				else ++it;
			}
		} while (!changed);
	}

	ArmyManager::ArmyManager()
		= default;

	ArmyManager & ArmyManager::instance()
	{
		static ArmyManager instance;
		return instance;
	}
}