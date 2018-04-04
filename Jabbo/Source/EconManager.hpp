#pragma once
#include <BWAPI.h>
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class EconManager
	{
	public:
		EconManager();
		static bool iHaveMoney(const UnitType& unit);
		bool iHaveMoney(const UnitType & unit, TilePosition start, TilePosition end) const;
		static double mineralGainPerFrame();
		static int getMineralsWhenReaching(Unit u, Position start, Position end);
		static int getMineralsWhenReaching(Position start, Position end);
		static EconManager & instance();
	};
}