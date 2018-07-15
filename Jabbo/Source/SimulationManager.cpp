#include "SimulationManager.hpp"
#include "Clustering.hpp"
#include <UnitInfoManager.h>
using namespace BWAPI;
using namespace Neolib;
using namespace scutil;
namespace Jabbo {
	void SimulationManager::createClusters()
	{
		// Friendly Clusters
		vector<Unit> myUnits{};
		for (const auto& u : UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->self()).getUnits())
		{
			if (u.first->isCompleted() && u.second.type.canAttack() && !u.second.type.isWorker())
			{
				myUnits.emplace_back(u.first);
			}
		}
		if (myUnits.empty()) return;
		// Enemy Clusters
		vector<Unit> enemyUnits{};
		for (const auto& u : UnitInfoManager::getInstance().getUnitDataOfPlayer(Broodwar->enemy()).getUnits())
		{
			if (u.first->isCompleted() && u.second.type.canAttack() && (!u.second.type.isWorker() || u.first->isAttacking()))
			{
				enemyUnits.emplace_back(u.first);
			}
		}
		if (enemyUnits.empty()) return;
		Clustering friendlyClustering(myUnits);
		friendly_ = friendlyClustering.run();
		Clustering enemyClustering(enemyUnits);
		enemies_ = enemyClustering.run();
	}

	bool SimulationManager::noNeedForSim()
	{
		return false;
	}

	double broodWarDistance(const pair<double, double> pos1, const pair<double, double> pos2)
	{
		const auto dx = abs(pos1.first - pos2.first);
		const auto dy = abs(pos1.second - pos2.second);
		const auto d = min(dx, dy);
		const auto D = max(dx, dy);
		if (d < D / 4) {
			return D;
		}
		return D - D / 16 + d * 3 / 8 - D / 64 + d * 3 / 256;
	}
	void SimulationManager::createSimInfos()
	{
		for (const auto& ally : friendly_) {
			if (ally.units.empty()) continue;
			SimInfo aux{};
			for (auto enemy : enemies_) {
				if (enemy.units.empty()) continue;
				if (broodWarDistance(ally.mode(), enemy.mode()) <= radius_) {
					aux.enemies.insert(enemy.units.begin(), enemy.units.end());
				}
			}
			if (!aux.enemies.empty()) {
				aux.allies.insert(ally.units.begin(), ally.units.end());
				simulations_.emplace_back(aux);
			}
		}
		vector<SimInfo> newSims{};
		for (const auto& s : simulations_) {
			SimInfo air{};
			SimInfo ground{};
			for (auto u : s.allies) {
				if (u->isFlying()) air.allies.emplace(u);
				if (u->getType().groundWeapon() != WeaponTypes::None) ground.allies.emplace(u);
			}
			const auto emptyAir = air.allies.empty();
			const auto emptyGround = ground.allies.empty();
			if (emptyAir && emptyGround) continue;
			for (const auto& u : s.enemies) {
				if (u->getType().airWeapon() != WeaponTypes::None) air.enemies.emplace(u);
				if (u->getType().groundWeapon() != WeaponTypes::None) ground.enemies.emplace(u);
			}
			if (!emptyAir && !air.enemies.empty()) {
				air.type = Air;
				newSims.emplace_back(air);
			}
			if (!emptyGround && !ground.enemies.empty()) {
				ground.type = Ground;
				newSims.emplace_back(ground);
			}
		}
		if (!newSims.empty()) simulations_.insert(simulations_.end(), newSims.begin(), newSims.end());
	}

	void SimulationManager::doSim()
	{
		for (auto& s : simulations_) {
			fap.clear();
			for (const auto& u : s.allies) {
				FastAPproximation::FAPUnit fU(u);
				fap.addUnitPlayer1(fU);
				s.stateBefore.first->emplace_back(fU);
			}
			for (const auto& u : s.enemies) {
				if (!u->isDetected()) {
					s.lose = true;
					break;
				}
				FastAPproximation::FAPUnit fU(u);
				fap.addUnitPlayer2(fU);
				s.stateBefore.second->emplace_back(fU);
			}
			if (s.lose) continue;
			s.preSimScore = fap.playerScores();
			fap.simulate(shortSimFrames_);
			s.postSimScore = fap.playerScores();
			const auto ourLosses = s.preSimScore.first - s.postSimScore.first;
			const auto enemyLosses = s.preSimScore.second - s.postSimScore.second;
			if (enemyLosses - ourLosses >= 0) return;
			fap.simulate(longSimFrames_);
			s.postSimScore = fap.playerScores();
			s.stateAfter = fap.getState();
			//Bad lose sim logic, testing
			s.lose = !scoreCalc(s, 3) || s.stateAfter.first->empty();
		}
	}

	bool SimulationManager::scoreCalc(const SimInfo s, const double rate)
	{
		return ((s.preSimScore.second - s.postSimScore.second) * rate <= (s.preSimScore.first - s.postSimScore.first));
	}

	void SimulationManager::drawCluster(Cluster c, const bool ally, const int id) const
	{
		auto color = Colors::Red;
		if (ally) color = Colors::Green;
		const auto centroid = Position(static_cast<int>(c.modeX), static_cast<int>(c.modeY));
		Broodwar->drawCircleMap(centroid, 5, color, true);
		auto idS = std::to_string(id);
		Broodwar->drawTextMap(centroid + Position(0, 5), idS.c_str());
		for (const auto& u : c.units) Broodwar->drawLineMap(u->getPosition(), centroid, color);
	}

	SimulationManager::SimulationManager()
		= default;

	bool SimulationManager::simulateHarass(set<UnitInfo>& enemies, const UnitInfo scouter, const int frames)
	{
		fap.clear();
		fap.addUnitPlayer1(FastAPproximation::FAPUnit(scouter));
		for (const auto& enemy : enemies)
		{
			fap.addUnitPlayer2(FastAPproximation::FAPUnit(enemy));
		}
		const int preSimFriendlyUnitCount = fap.getState().first->size();
		fap.simulate(frames);
		const int postSimFriendlyUnitCount = fap.getState().first->size();
		const auto myLosses = preSimFriendlyUnitCount - postSimFriendlyUnitCount;
		return myLosses == 0;
	}

	void SimulationManager::reset()
	{
		this->friendly_.clear();
		this->enemies_.clear();
		fap.clear();
		this->simulations_.clear();
	}

	void SimulationManager::onFrameSim()
	{
		const auto start = std::chrono::high_resolution_clock::now();
		reset();
		if (!noNeedForSim())
		{
			createClusters();
			//createSimInfos();
			//doSim();
		}
		const auto end = std::chrono::high_resolution_clock::now();
		time = long(std::chrono::duration_cast<chrono::milliseconds>(end - start).count());
	}

	void SimulationManager::drawClusters()
	{
		auto cluster = 0;
		for (const auto& c : friendly_) {
			drawCluster(c, true, cluster);
			cluster++;
		}
		cluster = 0;
		for (const auto& c : enemies_) {
			drawCluster(c, false, cluster);
			cluster++;
		}
	}

	SimInfo SimulationManager::getSimulation(const Unit unit, const simType type)
	{
		for (const auto& s : simulations_) {
			if (s.type == type && s.allies.find(unit) != s.allies.end()) return s;
		}
		return SimInfo();
	}

	SimInfo SimulationManager::getSimulation(const Unit unit, const bool enemy)
	{
		for (const auto& s : simulations_) {
			if (!enemy && s.allies.find(unit) != s.allies.end()) return s;
			if (enemy && s.enemies.find(unit) != s.enemies.end()) return s;
		}
		return SimInfo();
	}
}