#pragma once

#include <BWAPI.h>
#include <set>
#include"BWEM 1.4.1/src/bwem.h"

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	using namespace BWEM;

	enum team
	{
		Ally, Enemy, Neutral
	};

	struct Owner
	{
		team team = Neutral;
		Player player = Broodwar->neutral();
	};

	inline const char* toString(const team v) // TODO Finish change to struct owner, fix other classes
	{
		switch (v)
		{
		case Ally:   return "Ally";
		case Enemy:   return "Enemy";
		case Neutral: return "Neutral";
		default:      return "Neutral";
		}
	}
	struct Base
	{
		Base() = default;
		const BWEM::Base * base{};
		TilePosition tile{};
		set<Unit> minerals{};
		map<Unit, bool> geysers{};
		const Area * area{};
		Owner owner{};
		bool starting = false;

		bool operator<(const Base &cBase) const {
			return tile < cBase.tile;
		}
		bool operator==(const Base &cBase) const {
			return tile == cBase.tile;
		}
	};

	class BaseManager
	{
	public:
		set<Base> bases;
		BaseManager();
		static int getFriendlyBasesCount();
		static void initBaseManager();
		static BaseManager & instance();
	};
}