#pragma once

#include <map>
#include <BWAPI.h>
#include "UnitData.h"
namespace scutil {
	class UnitInfoManager {
		/** Stores the unit data per player  */
		std::map<BWAPI::Player, UnitData> _unitData;

		/** Draws a box around a unit that is not visible anymore */
		void drawUnit(UnitInfo& unitInfo);

		/** Private constructor, for singleton */
		UnitInfoManager();
	
	public:
		/** Returns the singleton instance of this class */
		static UnitInfoManager& getInstance();

		/** Default destructor */
		~UnitInfoManager();
	
		/** Returns unit data of a player */
		const UnitData & getUnitDataOfPlayer(BWAPI::Player player);

		/** Returns the map(Unit -> UnitInfo) of a given player */
		const	UIMap & getUnitInfoMapOfPlayer(BWAPI::Player player);

		/** Returns the map (Player -> UnitData) with all information */
		const std::map<BWAPI::Player, UnitData>& getAllUnitData() const;

		/** Call this at the onStart of your AIModule */
		void onStart();

		/** Call this at the onFrame of your AIModule */
		void onFrame();

		/** Call this at the onUnitDiscover of your AIModule */
		void onUnitDiscover(BWAPI::Unit unit);

		/** Call this at the onUnitEvade of your AIModule */
		void onUnitEvade(BWAPI::Unit unit);

		/** Call this at the onUnitShow of your AIModule */
		void onUnitShow(BWAPI::Unit unit);

		/** Call this at the onUnitHide of your AIModule */
		void onUnitHide(BWAPI::Unit unit);

		/** Call this at the onUnitCreate of your AIModule */
		void onUnitCreate(BWAPI::Unit unit);

		/** Call this at the onUnitDestroy of your AIModule */
		void onUnitDestroy(BWAPI::Unit unit);

		/** Call this at the onUnitMorph of your AIModule */
		void onUnitMorph(BWAPI::Unit unit);

		/** Call this at the onUnitRenegade of your AIModule */
		void onUnitRenegade(BWAPI::Unit unit);

		/** Call this at the onUnitComplete of your AIModule */
		void onUnitComplete(BWAPI::Unit unit);
	};
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
