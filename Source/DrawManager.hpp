#pragma once
#include "UnitInfoManager.h"
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class DrawManager
	{
	public:
		static void onFrame();
		static void drawUnit(const Unit& unit, const Color color);
		static void drawTextUnit(Position pos, string& text);
		static void drawBoxUnit(const TilePosition top, const UnitType type, Color color);
		DrawManager();
		static DrawManager & instance();
	};
}