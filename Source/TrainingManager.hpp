#pragma once

#include <BWAPI.h>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class TrainingManager
	{
	public:
		TrainingManager();
		static TrainingManager & Instance();
		static bool iHaveMoney(const UnitType & unit);
		static void onFrame();
	};
}