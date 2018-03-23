#pragma once

#include <BWAPI.h>
#include <vector>
#include <string>

using namespace BWAPI;
using namespace std;

struct BOItem {
	int supply{};
	UnitType unit;
};

struct BuildOrder {
	vector< BOItem > itemsBO;
	string name;
	Race enemyRace;
	bool wall{};//if a wall-in is required
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