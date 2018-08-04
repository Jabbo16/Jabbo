#include "Squad.hpp"
#include <set>

namespace Jabbo
{
	Squad::Squad(scutil::UnitInfo u, const int id)
	{
		this->members.emplace(u.unit, u);
		this->center = u.lastPosition;
		this->id = id;
	}

	void Squad::mainAttack() const
	{
	}

	void Squad::idle() const
	{
	}

	void Squad::defense() const
	{
	}

	void Squad::explore() const
	{
	}

	void Squad::drop() const
	{
	}

	void Squad::harass() const
	{
	}

	void Squad::run() const
	{
		if (this->members.empty()) return;
		switch (status) {
		case MainAttack:
			mainAttack();
			break;
		case Idle:
			idle();
			break;
		case Defense:
			defense();
			break;
		case Explore:
			explore();
			break;
		case Drop:
			drop();
			break;
		case Harass:
			harass();
			break;
		default:;
		}
	}

	void Squad::updateSquadCenter() const
	{
		if (this->members.empty()) return;
		if (this->members.size() == 1) {
			this->center = this->members.begin()->second.lastPosition;
			return;
		}
		Position c{ 0,0 };
		for ([[maybe_unused]] auto[key, value] : this->members)
		{
			c += value.lastPosition;
		}
		this->setCenter(c / this->members.size());
	}

	void Squad::setCenter(const Position center) const
	{
		this->center = center;
	}

	void Squad::getLineFormation(const Position start, const int width) const
	{
		if (members.empty()) return;
		const auto marineWidth = static_cast<int>(UnitTypes::Terran_Marine.width() * 1.5);
		const auto marineHeight = static_cast<int>(UnitTypes::Terran_Marine.height() * 1.5);
		if (width < marineWidth && members.size() != 1) return;
		const auto numPos = members.size();
		const signed int totalWidth = marineWidth * numPos;
		const auto numRows = totalWidth > width ? ceil(totalWidth / width) : 1;
		Position pos{};
		const auto numPosRow = numPos / numRows;
		set<Position> marinePos{};
		for (auto row = 0; row < numRows; row++)
		{
			unsigned int i = 1;
			const auto center = start - Position(0, row * marineHeight);
			marinePos.emplace(center);
			for (; i <= ceil((numPosRow - 1) / 2); i++)
			{
				if (marinePos.size() >= numPos) break;
				pos = center - Position(i * marineWidth, 0);
				marinePos.emplace(pos);
			}
			i = 1;
			for (; i <= ceil((numPosRow - 1) / 2); i++)
			{
				if (marinePos.size() >= numPos) break;
				pos = center + Position(i * marineWidth, 0);
				marinePos.emplace(pos);
			}
		}
		for (const auto&[unit, unitInfo] : members)
		{
			auto distMax = DBL_MAX;
			Position chosen{};
			for (const auto& p : marinePos)
			{
				const auto dist = unitInfo.lastPosition.getDistance(p);
				if (dist < distMax)
				{
					chosen = p;
					distMax = dist;
				}
			}
			if (chosen)
			{
				unit->move(chosen);
				marinePos.erase(chosen);
			}
		}
	}
}