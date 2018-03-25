#pragma once

#include <BWAPI.h>
#include <set>
#include"BWEM 1.4.1/src/bwem.h"
namespace { auto & bwem = BWEM::Map::Instance(); }

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	using namespace BWEM;

	enum owner
	{
		Ally, Enemy, Neutral
	};
	inline const char* toString(const owner v)
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
		const BWEM::Area * area{};
		owner owner = Neutral;
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