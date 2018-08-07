#include "BuildOrderManager.hpp"
#include "InfoManager.hpp"
using namespace Jabbo;

void BuildOrderManager::bioBuild() {
	myBo.enemyRace = Races::Terran;
	myBo.name = "bioBuild";
	BOItem addPiece;
	addPiece.supply = 8;
	addPiece.type = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 10;
	addPiece.type = UnitTypes::Terran_Barracks;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 12;
	addPiece.type = UnitTypes::Terran_Barracks;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 15;
	addPiece.type = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);
	addPiece.supply = 18;
	addPiece.type = UnitTypes::Terran_Academy;
	myBo.itemsBO.push_back(addPiece);
}

void BuildOrderManager::bbs() {
	myBo.enemyRace = Races::Terran;
	myBo.name = "bbs";
	BOItem addPiece;

	addPiece.supply = 8;
	addPiece.type = UnitTypes::Terran_Barracks;
	addPiece.amount = 2;
	myBo.itemsBO.push_back(addPiece);

	addPiece.amount = 1;
	addPiece.supply = 8;
	addPiece.type = UnitTypes::Terran_SCV;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 9;
	addPiece.type = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 10;
	addPiece.type = UnitTypes::Terran_SCV;
	myBo.itemsBO.push_back(addPiece);

	addPiece.supply = 14;
	addPiece.type = UnitTypes::Terran_Supply_Depot;
	myBo.itemsBO.push_back(addPiece);
}

void BuildOrderManager::onStart() {
	if (InfoManager::instance().myRace == Races::Terran) bbs();
}

BuildOrderManager::BuildOrderManager() = default;

BuildOrderManager & BuildOrderManager::instance()
{
	static BuildOrderManager instance;
	return instance;
}