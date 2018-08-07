#pragma once

#include <BWAPI.h>
#include <vector>
#include <string>
#include <variant>

using namespace BWAPI;
using namespace std;

enum buildingPlacement
{
	Default, Proxy, Hidden
};

enum wallPlacement
{
	Main, Natural
};

struct BOItem {
	int supply{};
	variant<UnitType, TechType, UpgradeType> type;
	int amount = 1;
	buildingPlacement placement = Default;
};

struct BuildOrder {
	vector<BOItem> itemsBO;
	string name;
	Race myRace;
	Race enemyRace;
	wallPlacement wallPlacement = Main;
};

class BuildOrderManager {
public:
	BuildOrderManager();
	static BuildOrderManager& instance();
	BuildOrder myBo;
	void bioBuild();
	void bbs();
	void onStart();
};