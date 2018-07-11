#include "Jabbo.hpp"
#include "BuildingManager.hpp"
#include "DrawManager.hpp"
#include "RecollectManager.hpp"
#include "Agents/WorkerScouterAgent.hpp"
#include "ScoutingManager.hpp"
#include "InfoManager.hpp"
#include "ResourceManager.hpp"
#include "TrainingManager.hpp"
#include "BWEB/BWEB.h"
#include <chrono>
using namespace BWAPI;
using namespace scutil;
using namespace Filter;
using namespace BWEM;
using namespace BWAPI_ext;
using namespace utils;
namespace { auto & bwem = Map::Instance(); }
namespace { auto & mapBweb = BWEB::Map::Instance(); }
using namespace Jabbo;
bool scouting = false;
void JabboModule::onStart()
{
	bwem.Initialize();
	bwem.EnableAutomaticPathAnalysis();
	bwem.FindBasesForStartingLocations();
	MapPrinter::Initialize(&bwem);

	mapBweb.onStart();
	std::vector<UnitType> wall = { UnitTypes::Terran_Supply_Depot, UnitTypes::Terran_Barracks, UnitTypes::Terran_Bunker };

	const auto start = chrono::high_resolution_clock::now();
	mapBweb.createWall(wall, mapBweb.getNaturalArea(), mapBweb.getNaturalChoke(), UnitTypes::Zerg_Zergling);
	const auto dur = std::chrono::duration <double, std::milli>(std::chrono::high_resolution_clock::now() - start).count();

	Broodwar << "A* with potato time:" << dur << endl;
	mapBweb.findBlocks();
	UnitInfoManager::getInstance().onStart();
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setLocalSpeed(1);
	//	Broodwar->setFrameSkip(1000);
	BaseManager::initBaseManager();
	InfoManager::onStart();
	ResourceManager::initBaseResources();
	RecollectManager::initTree();
	BuildingManager::initTree();
	const Position test{ 64,64 };
	const auto testW = WalkPosition(test);
	Broodwar->sendText("%d, %d", testW.x, testW.y);
	// Set the command optimization level so that common commands can be grouped and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);
}

void JabboModule::onEnd(const bool isWinner)
{
	if (isWinner)
	{
		// Log your win here!
	}
}

void JabboModule::onFrame()
{
	// Called once every game frame
	// mapBweb.draw();

	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
	{
		// activates the onFrame of UnitInfoManager
		UnitInfoManager::getInstance().onFrame();
		DrawManager::onFrame();
		return;
	}

	// activates the onFrame of UnitInfoManager
	UnitInfoManager::getInstance().onFrame();
	if (!ScoutingManager::instance().initialScoutingDone && Broodwar->self()->supplyUsed() / 2 >= 9)
	{
		ScoutingManager::firstScouting();
	}
	ScoutingManager::onFrame();
	BuildingManager::onFrame();
	RecollectManager::onFrame();
	TrainingManager::onFrame();
	DrawManager::onFrame();
	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
}

void JabboModule::onSendText(std::string text)
{
	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());

	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!
}

void JabboModule::onReceiveText(const BWAPI::Player player, const std::string text)
{
	MapDrawer::ProcessCommand(text);
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << "\n";
}

void JabboModule::onPlayerLeft(const BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void JabboModule::onNukeDetect(const BWAPI::Position target)
{
	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << "\n";
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void JabboModule::onUnitDiscover(const BWAPI::Unit unit)
{
	mapBweb.onUnitDiscover(unit);
	UnitInfoManager::getInstance().onUnitDiscover(unit);
}

void JabboModule::onUnitEvade(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitEvade(unit);
}

void JabboModule::onUnitShow(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitShow(unit);
}

void JabboModule::onUnitHide(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitHide(unit);
}

void JabboModule::onUnitCreate(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitCreate(unit);
	if (unit->getPlayer() == Broodwar->self()) {
		if (unit->getType().isBuilding())
		{
			BuildingManager::onUnitCreate(unit);
		}
	}
}

void JabboModule::onUnitDestroy(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitDestroy(unit);
	mapBweb.onUnitDestroy(unit);
	if (unit->getType().isMineralField())
	{
		ResourceManager::onMineralDestroy(unit);
		bwem.OnMineralDestroyed(unit);
	}
	else if (unit->getType().isSpecialBuilding())
	{
		bwem.OnStaticBuildingDestroyed(unit);
	}
	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType().isWorker())
		{
			RecollectManager::onUnitDestroy(unit);
			ScoutingManager::onUnitDestroy(unit);
		}

		BuildingManager::onUnitDestroy(unit);
	}
}

void JabboModule::onUnitMorph(const BWAPI::Unit unit)
{
	if (unit->getType().isRefinery() && unit->getPlayer() == Broodwar->self())
	{
		BuildingManager::onUnitCreate(unit);
	}
	mapBweb.onUnitMorph(unit);
	UnitInfoManager::getInstance().onUnitMorph(unit);
}

void JabboModule::onUnitRenegade(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitRenegade(unit);
}

void JabboModule::onSaveGame(const std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << "\n";
}

void JabboModule::onUnitComplete(const BWAPI::Unit unit)
{
	UnitInfoManager::getInstance().onUnitComplete(unit);
	if (unit->getPlayer() == Broodwar->self())
	{
		if (unit->getType().isWorker())
		{
			RecollectManager::onUnitComplete(unit);
			return;
		}
		if (unit->getType().isBuilding())
		{
			BuildingManager::onUnitComplete(unit);
		}
	}
}