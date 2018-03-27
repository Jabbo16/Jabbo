#include "BuildOrderManager.hpp"

void BuildOrderManager::oneGateCore() {
	myBo.enemyRace = Races::Terran;
	myBo.name = "1 Gate Core";
	BOItem addPiece;
	addPiece.supply = 8;
	addPiece.unit = UnitTypes::Protoss_Pylon;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 10;
	addPiece.unit = UnitTypes::Protoss_Gateway;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 12;
	addPiece.unit = UnitTypes::Protoss_Assimilator;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 14;
	addPiece.unit = UnitTypes::Protoss_Cybernetics_Core;
	myBo.itemsBO.push_back(addPiece);
}

void BuildOrderManager::fourGate() {
	myBo.enemyRace = Races::Terran;
	myBo.name = "4 Gate";
	BOItem addPiece;

	addPiece.supply = 8;
	addPiece.unit = UnitTypes::Protoss_Pylon;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 10;
	addPiece.unit = UnitTypes::Protoss_Gateway;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 12;
	addPiece.unit = UnitTypes::Protoss_Pylon;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 13;
	addPiece.unit = UnitTypes::Protoss_Zealot;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 16;
	addPiece.unit = UnitTypes::Protoss_Assimilator;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 17;
	addPiece.unit = UnitTypes::Protoss_Cybernetics_Core;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 18;
	addPiece.unit = UnitTypes::Protoss_Zealot;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 22;
	addPiece.unit = UnitTypes::Protoss_Pylon;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 23;
	addPiece.unit = UnitTypes::Protoss_Dragoon;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 25;
	addPiece.unit = UnitTypes::Protoss_Gateway;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 25;
	addPiece.unit = UnitTypes::Protoss_Gateway;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 25;
	addPiece.unit = UnitTypes::Protoss_Gateway;
	myBo.itemsBO.push_back(addPiece);
}

void BuildOrderManager::bioBuild() {
	myBo.enemyRace = Races::Terran;
	myBo.name = "bioBuild";
	BOItem addPiece;
	addPiece.supply = 8;
	addPiece.unit = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 10;
	addPiece.unit = UnitTypes::Terran_Barracks;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 12;
	addPiece.unit = UnitTypes::Terran_Barracks;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 15;
	addPiece.unit = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 18;
	addPiece.unit = UnitTypes::Terran_Academy;
	myBo.itemsBO.push_back(addPiece);
}

BuildOrderManager::BuildOrderManager() {
	//	if (Broodwar->self()->getRace() == Races::Protoss) oneGateCore();
	if (Broodwar->self()->getRace() == Races::Protoss) fourGate();
	if (Broodwar->self()->getRace() == Races::Terran) bioBuild();
}

BuildOrderManager & BuildOrderManager::instance()
{
	static BuildOrderManager instance;
	return instance;
}