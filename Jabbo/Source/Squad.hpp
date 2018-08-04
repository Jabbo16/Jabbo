#pragma once
#include <BWAPI.h>
#include <UnitInfoManager.h>
#include <BWAPI/AIModule.h>

namespace Jabbo {
	using namespace BWAPI;
	using namespace std;
	class Squad {
	public:
		enum status
		{
			MainAttack, Idle, Defense, Explore, Drop, Harass
		};
		bool operator<(const Squad &s) const {
			return id < s.id;
		}
		bool operator==(const Squad &s) const {
			return id == s.id;
		}
		Squad(scutil::UnitInfo u, int id);
		void run() const;
		void setCenter(Position center) const;
		void getLineFormation(Position start, int width) const;
		void updateSquadCenter() const;
		mutable map<Unit, scutil::UnitInfo> members{};
		int id{};
		status status{ Idle };
		mutable Position center{};
	private:
		void mainAttack() const;
		void idle() const;
		void defense() const;
		void explore() const;
		void drop() const;
		void harass() const;
	};
}