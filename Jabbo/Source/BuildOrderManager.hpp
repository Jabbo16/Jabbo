#pragma once

#include <BWAPI.h>
#include <vector>
#include <string>
#include <variant>

using namespace BWAPI;
using namespace std;

struct BOItem {
	int supply{};
	variant<UnitType, TechType, UpgradeType> type;
};

struct BuildOrder {
	vector< BOItem > itemsBO;
	string name;
	Race enemyRace;
	bool wall{};//if a wall-in is required, maybe vector of unittypes at walls that could be empty? // TODO read comment
};

class BuildOrderManager {
public:
	BuildOrderManager();
	static BuildOrderManager& instance();
	BuildOrder myBo;
	void oneGateCore();
	void fourGate();
	void bioBuild();
};