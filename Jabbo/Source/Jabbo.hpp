#pragma once

#include <BWAPI.h>
#include"BWEM 1.4.1/src/bwem.h"
#include "BWEB/BWEB.h"
#include "ResourceManager.hpp"
#include "RecollectManager.hpp"
#include "TrainingManager.hpp"
#include "UnitInfoManager.h"
#include <string>
#include <iostream>
#include <vector>

namespace Jabbo
{
	class JabboModule : public BWAPI::AIModule
	{
	public:

		void onStart() override;
		void onEnd(bool isWinner) override;
		void onFrame() override;
		void onSendText(std::string text) override;
		void onReceiveText(Player player, std::string text) override;
		void onPlayerLeft(Player player) override;
		void onNukeDetect(Position target) override;
		void onUnitDiscover(Unit unit) override;
		void onUnitEvade(Unit unit) override;
		void onUnitShow(Unit unit) override;
		void onUnitHide(Unit unit) override;
		void onUnitCreate(Unit unit) override;
		void onUnitDestroy(Unit unit) override;
		void onUnitMorph(Unit unit) override;
		void onUnitRenegade(Unit unit) override;
		void onSaveGame(std::string gameName) override;
		void onUnitComplete(Unit unit) override;
	};
}