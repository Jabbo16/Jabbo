#include "DrawManager.hpp"
#include "ResourceManager.hpp"
#include "RecollectManager.hpp"
#include "BuildingManager.hpp"
#include "TrainingManager.hpp"
using namespace scutil;
namespace Jabbo {
	DrawManager::DrawManager()
		= default;

	DrawManager & DrawManager::instance()
	{
		static DrawManager instance;
		return instance;
	}

	void DrawManager::onFrame()
	{
		for (const auto m : ResourceManager::instance().minerals)
		{
			string text = to_string(m.second);
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
			drawTextUnit(m.first->getPosition(), text);
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

		// Display the game frame rate as text in the upper left area of the screen
		Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
		Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
		Broodwar->drawTextScreen(200, 60, "%s", Broodwar->getLastError().c_str());

		if (!BuildOrderManager::instance().myBo.itemsBO.empty())
		{
			Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: %s",
				BuildOrderManager::instance().myBo.itemsBO[0].unit.getName().c_str());
		}
		else
		{
			Broodwar->drawTextScreen(200, 40, "Next Item in BO Queue: None");
		}
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
		const Position leftTop = Position(top.x * 32, top.y * 32);
		const Position rightBottom = Position(leftTop.x + 32 + type.tileWidth(), leftTop.y + 32 + type.tileHeight());
		Broodwar->drawBoxMap(leftTop, rightBottom, color);
	}
}