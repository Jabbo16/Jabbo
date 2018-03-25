#pragma once

#include <BWAPI.h>
#include <set>
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	struct UnitItemQueue
	{
		Unit building = nullptr;
		UnitType type = UnitTypes::Unknown;
		int frameCompletion = -1;
		bool isFromBO = false;
	};
	class TrainingManager
	{
		map <UnitType, double> unitScore;
		map <UnitType, double>& getUnitScore() { return unitScore; }
	public:
		TrainingManager();
		vector<UnitItemQueue> unitQueue{};
		static TrainingManager & instance();
		static bool iHaveMoney(const UnitType & unit);
		void updateScoring();
		void updateProtossUnitScore(UnitType unit, int count);
		static void onFrame();
		static bool hasUnitTypeRequirement(BWAPI::UnitType unit_type);
	};
}