#pragma once

#include <BWAPI.h>
namespace scutil {
	/** Stores information of a game unit, persisting when unit is not visible */
	struct UnitInfo {
	    // we need to store all of this data because if the unit is not visible, we
	    // can't reference it from the unit pointer
	
	    int             unitID;
	    int             lastHealth;
	    int             lastShields;
	    BWAPI::Player   player;
	    BWAPI::Unit     unit;
	    BWAPI::Position lastPosition;
	    BWAPI::UnitType type;
	    bool            completed;
	
	    UnitInfo()
	        : unitID(0)
	        , lastHealth(0)
	        , player(nullptr)
	        , unit(nullptr)
	        , lastPosition(BWAPI::Positions::None)
	        , type(BWAPI::UnitTypes::None)
	        , completed(false)
	    {
	
	    }
	
	    const bool operator == (BWAPI::Unit unit) const
	    {
	        return unitID == unit->getID();
	    }
	
	    const bool operator == (const UnitInfo & rhs) const
	    {
	        return (unitID == rhs.unitID);
	    }
	
	    const bool operator < (const UnitInfo & rhs) const
	    {
	        return (unitID < rhs.unitID);
	    }
	};
	
	typedef std::vector<UnitInfo> UnitInfoVector;
	typedef std::map<BWAPI::Unit,UnitInfo> UIMap;
	
	/** Stores data of game units as well as some global statistics */
	class UnitData {
	    UIMap unitMap;
	
		/** returns whether unit information is inconsistent */
	    const bool badUnitInfo(const UnitInfo & ui) const;
	
	    std::vector<int>						numDeadUnits;
	    std::vector<int>						numUnits;
	
	    int										mineralsLost;
	    int										gasLost;
	
	public:

		UnitData();

		/** Updates data of a unit (call when you see the unit) */
		void	updateUnit(BWAPI::Unit unit);

		/** Removes a unit from the registry (call when the unit was killed or renegade) */
		void	removeUnit(BWAPI::Unit unit);

		/** Removes the units with inconsistent data */
		void	removeBadUnits();

		/** Returns how many gas was lost due to dead units */
		int		getGasLost()                                const;

		/** Returns how many minerals were was lost due to dead units */
		int		getMineralsLost()                           const;

		/** Counts the number of units of a given type */
		int		getNumUnits(BWAPI::UnitType t)              const;

		/** Counts the number of dead units of a given type */
		int		getNumDeadUnits(BWAPI::UnitType t)          const;

		/** Returns the Unit Info. map. (BWAPI::Unit -> UnitInfo) */
		const	UIMap & getUnits() const;

	};
}	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
