#pragma once
#include <BWAPI.h>
#include <set>
#include "FAP/FAP.h"
#include <chrono>
#include "Clustering.hpp"

namespace Jabbo {
	class Cluster;
	using namespace BWAPI;
	using namespace std;
	using namespace Neolib;
	using Neolib::FastAPproximation;
	enum simType { Ground, Air, Mix };

	struct SimInfo
	{
		simType type{ Mix };
		set<Unit> allies{};
		set<Unit> enemies{};
		pair<int, int> preSimScore{};
		pair<int, int> postSimScore{};
		pair<vector<FastAPproximation::FAPUnit>*, vector<FastAPproximation::FAPUnit>*> stateBefore{};
		pair<vector<FastAPproximation::FAPUnit>*, vector<FastAPproximation::FAPUnit>*> stateAfter{};
		bool lose{};
	};

	class SimulationManager
	{
	private:
		const double radius_ = UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange();
		vector<Cluster> friendly_{};
		vector<Cluster> enemies_{};
		vector<SimInfo> simulations_{};
		int shortSimFrames_ = 90;
		int longSimFrames_ = 300;
		void createClusters();
		static bool noNeedForSim();
		void createSimInfos();
		void doSim();
		static bool scoreCalc(SimInfo s, double rate);
		void drawCluster(Cluster c, bool ally, int id) const;

	public:
		long time{};
		SimulationManager();
		static bool simulateHarass(set<scutil::UnitInfo> &enemies, scutil::UnitInfo scouter, int frames);
		void reset();
		void onFrameSim();
		void drawClusters();
		SimInfo getSimulation(Unit, simType type);
		SimInfo getSimulation(Unit unit, bool enemy = false);
	};
}