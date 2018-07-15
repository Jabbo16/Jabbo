#include "DrawManager.hpp"
#include "ResourceManager.hpp"
#include "RecollectManager.hpp"
#include "BuildingManager.hpp"
#include "ScoutingManager.hpp"
#include "InfoManager.hpp"
#include "EconManager.hpp"
#include "BWEB/BWEB.h"
#include "SimulationManager.hpp"
using namespace scutil;
namespace { auto& mapBweb = BWEB::Map::Instance(); }
namespace Jabbo {
	DrawManager::DrawManager()
		= default;

	DrawManager & DrawManager::instance()
	{
		static DrawManager instance;
		return instance;
	}

	void DrawManager::onFrame(SimulationManager s)
	{
		mapBweb.draw();

		for (const auto m : ResourceManager::instance().minerals)
		{
			auto text = to_string(m.second);
			drawTextUnit(m.first->getPosition(), text);
		}

		for (const auto m : RecollectManager::instance().workerMineral)
		{
			string text = "Miner";
			drawTextUnit(m.first->getPosition(), text);
			Broodwar->drawLineMap(m.first->getPosition(), m.second->getPosition(), Colors::Red);
		}

		for (const auto g : RecollectManager::instance().workerGas)
		{
			if (g.first->getOrder() == Orders::HarvestGas)
			{
				continue;
			}
			string text = "Gaser";
			drawTextUnit(g.first->getPosition(), text);
		}

		for (const auto m : RecollectManager::instance().workerIdle)
		{
			string text = "Idle";
			drawTextUnit(m->getPosition(), text);
		}

		for (const auto m : BuildingManager::instance().workerBuild)
		{
			string text = "Builder";
			string textType = m.second.type.c_str();
			drawTextUnit(m.first->getPosition(), text);
			drawTextUnit(Position(m.second.pos), textType);
			drawBoxUnit(m.second.pos, m.second.type, Colors::Yellow);
		}

		for (const auto m : BuildingManager::instance().workerTask)
		{
			string text = "Tasked";
			drawTextUnit(m.second->getPosition(), text);
		}

		for (const auto enemy : UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->enemy()).getUnits())
		{
			drawUnit(enemy.first, Colors::Red);
		}

		for (auto &scouter : ScoutingManager::instance().workerScouters)
		{
			if (scouter.currentObjective) drawBoxUnit(scouter.currentObjective->tile, Broodwar->enemy()->getRace().getResourceDepot(), Colors::Red);
			if (scouter.baseToHarass) drawTextUnit(scouter.myUnit->getPosition() + Position(0, 10), toString(scouter.harassStatus));
			string text = "Scouter";
			drawUnit(scouter.myUnit, Colors::Blue);
			drawTextUnit(scouter.myUnit->getPosition(), text);
		}

		if (InfoManager::instance().mainBase)
		{
			Broodwar->drawTextMap(Position(InfoManager::instance().mainBase->tile), "MainBase");
		}

		if (InfoManager::instance().mainNaturalBase)
		{
			Broodwar->drawTextMap(Position(InfoManager::instance().mainNaturalBase->tile), "MainNaturalBase");
		}
		if (InfoManager::instance().naturalChoke)
		{
			Broodwar->drawTextMap(Position(InfoManager::instance().naturalChoke.value()->Center()), "NaturalChoke");
		}
		if (InfoManager::instance().mainChoke)
		{
			Broodwar->drawTextMap(Position(InfoManager::instance().mainChoke.value()->Center()), "MainChoke");
		}
		// Display the game frame rate as text in the upper left area of the screen
		Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
		Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
		if (!BuildOrderManager::instance().myBo.itemsBO.empty())
		{
			if (std::holds_alternative<UnitType>(BuildOrderManager::instance().myBo.itemsBO[0].type)) Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: %s at %d supply",
				std::get<UnitType>(BuildOrderManager::instance().myBo.itemsBO[0].type).getName().c_str(), BuildOrderManager::instance().myBo.itemsBO[0].supply);
			else if (std::holds_alternative<TechType>(BuildOrderManager::instance().myBo.itemsBO[0].type)) Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: %s at %d supply",
				std::get<TechType>(BuildOrderManager::instance().myBo.itemsBO[0].type).getName().c_str(), BuildOrderManager::instance().myBo.itemsBO[0].supply);

			else if (std::holds_alternative<UpgradeType>(BuildOrderManager::instance().myBo.itemsBO[0].type)) Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: %s at %d supply",
				std::get<UpgradeType>(BuildOrderManager::instance().myBo.itemsBO[0].type).getName().c_str(), BuildOrderManager::instance().myBo.itemsBO[0].supply);
		}
		else
		{
			Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: None");
		}
		Broodwar->drawTextScreen(200, 60, "%s", Broodwar->getLastError().c_str());
		Broodwar->drawTextScreen(200, 80, "MGPF: %f", EconManager::mineralGainPerFrame());
		Broodwar->drawTextScreen(200, 100, "SimTime(ms): %i", s.time);
	}

	void DrawManager::drawUnit(const Unit &unit, const Color color)
	{
		Broodwar->drawBoxMap(unit->getLeft(), unit->getTop(), unit->getRight(), unit->getBottom(), color);
	}

	void DrawManager::drawTextUnit(const Position pos, string& text)
	{
		Broodwar->drawTextMap(pos, text.c_str());
	}

	void DrawManager::drawBoxUnit(const TilePosition top, const UnitType type, const Color color)
	{
		const auto leftTop = Position(top.x * 32, top.y * 32);
		const auto rightBottom = Position(leftTop.x + 32 + type.tileWidth(), leftTop.y + 32 + type.tileHeight());
		Broodwar->drawBoxMap(leftTop, rightBottom, color);
	}
}