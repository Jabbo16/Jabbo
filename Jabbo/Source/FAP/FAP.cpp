#include "FAP.h"
#include <UnitInfoManager.h>
#define MAX(a, b) (((a) < (b) ? (b) : (a)))
Neolib::FastAPproximation fap;

namespace Neolib {
	using namespace scutil;
	FastAPproximation::FastAPproximation() = default;

	void FastAPproximation::addUnitPlayer1(FAPUnit const fu) { player1.push_back(fu); }

	void FastAPproximation::addIfCombatUnitPlayer1(FAPUnit const fu) {
		if (fu.unitType == BWAPI::UnitTypes::Protoss_Interceptor)
			return;
		if (fu.groundDamage || fu.airDamage || fu.unitType == BWAPI::UnitTypes::Terran_Medic)
			addUnitPlayer1(fu);
	}

	void FastAPproximation::addUnitPlayer2(FAPUnit const fu) { player2.push_back(fu); }

	void FastAPproximation::addIfCombatUnitPlayer2(FAPUnit const fu) {
		if (fu.groundDamage || fu.airDamage || fu.unitType == BWAPI::UnitTypes::Terran_Medic)
			addUnitPlayer2(fu);
	}

	void FastAPproximation::simulate(int nFrames) {
		while (nFrames--) {
			if (player1.empty() || player2.empty())
				break;

			didSomething_ = false;

			isimulate();

			if (!didSomething_)
				break;
		}
	}

	const auto score = [](const FastAPproximation::FAPUnit &fu) {
		if (fu.health && fu.maxHealth)
			return ((fu.score * fu.health) / (fu.maxHealth * 2)) +
			(fu.unitType == BWAPI::UnitTypes::Terran_Bunker) * BWAPI::UnitTypes::Terran_Marine.destroyScore() * 4;
		return 0;
	};

	std::pair<int, int> FastAPproximation::playerScores() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			res.first += score(u);

		for (const auto &u : player2)
			res.second += score(u);

		return res;
	}

	std::pair<int, int> FastAPproximation::playerScoresUnits() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			if (!u.unitType.isBuilding())
				res.first += score(u);

		for (const auto &u : player2)
			if (!u.unitType.isBuilding())
				res.second += score(u);

		return res;
	}

	std::pair<int, int> FastAPproximation::playerScoresBuildings() const {
		std::pair<int, int> res;

		for (const auto &u : player1)
			if (u.unitType.isBuilding())
				res.first += score(u);

		for (const auto &u : player2)
			if (u.unitType.isBuilding())
				res.second += score(u);

		return res;
	}

	std::pair<std::vector<FastAPproximation::FAPUnit> *, std::vector<FastAPproximation::FAPUnit> *> FastAPproximation::getState() {
		return { &player1, &player2 };
	}

	void FastAPproximation::clear() { player1.clear(), player2.clear(); }

	void FastAPproximation::dealDamage(const FastAPproximation::FAPUnit &fu, int damage, BWAPI::DamageType const damageType) {
		damage <<= 8;
		auto const remainingShields = fu.shields - damage + (fu.shieldArmor << 8);
		if (remainingShields > 0) {
			fu.shields = remainingShields;
			return;
		}
		else if (fu.shields) {
			damage -= fu.shields + (fu.shieldArmor << 8);
			fu.shields = 0;
		}

		if (!damage)
			return;

		damage -= fu.armor << 8;

		if (damageType == BWAPI::DamageTypes::Concussive) {
			if (fu.unitSize == BWAPI::UnitSizeTypes::Large)
				damage = damage / 4;
			else if (fu.unitSize == BWAPI::UnitSizeTypes::Medium)
				damage = damage / 2;
		}
		else if (damageType == BWAPI::DamageTypes::Explosive) {
			if (fu.unitSize == BWAPI::UnitSizeTypes::Small)
				damage = damage / 2;
			else if (fu.unitSize == BWAPI::UnitSizeTypes::Medium)
				damage = (damage * 3) / 4;
		}

		fu.health -= MAX(128, damage);
	}

	int inline FastAPproximation::distButNotReally(const FAPUnit &u1, const FAPUnit &u2) {
		return (u1.x - u2.x) * (u1.x - u2.x) + (u1.y - u2.y) * (u1.y - u2.y);
	}

	bool FastAPproximation::isSuicideUnit(BWAPI::UnitType const ut) {
		return (ut == BWAPI::UnitTypes::Zerg_Scourge ||
			ut == BWAPI::UnitTypes::Terran_Vulture_Spider_Mine ||
			ut == BWAPI::UnitTypes::Zerg_Infested_Terran ||
			ut == BWAPI::UnitTypes::Protoss_Scarab);
	}

	void FastAPproximation::unitsim(const FAPUnit &fu, std::vector<FAPUnit> &enemyUnits) {
		if (fu.attackCooldownRemaining) {
			didSomething_ = true;
			return;
		}

		auto closestEnemy = enemyUnits.end();
		auto closestDist = 0;

		for (auto enemyIt = enemyUnits.begin(); enemyIt != enemyUnits.end();
			++enemyIt) {
			if (enemyIt->flying) {
				if (fu.airDamage) {
					auto const d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.airMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
			else {
				if (fu.groundDamage) {
					auto const d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.groundMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
		}

		if (closestEnemy != enemyUnits.end() && sqrt(closestDist) <= fu.speed &&
			!(fu.x == closestEnemy->x && fu.y == closestEnemy->y)) {
			fu.x = closestEnemy->x;
			fu.y = closestEnemy->y;
			closestDist = 0;

			didSomething_ = true;
		}

		if (closestEnemy != enemyUnits.end() &&
			closestDist <=
			(closestEnemy->flying ? fu.groundMaxRange : fu.airMinRange)) {
			if (closestEnemy->flying)
				dealDamage(*closestEnemy, fu.airDamage, fu.airDamageType),
				fu.attackCooldownRemaining = fu.airCooldown;
			else {
				dealDamage(*closestEnemy, fu.groundDamage, fu.groundDamageType);
				fu.attackCooldownRemaining = fu.groundCooldown;
				if (fu.elevation != -1 && closestEnemy->elevation != -1)
					if (closestEnemy->elevation > fu.elevation)
						fu.attackCooldownRemaining += fu.groundCooldown;
			}

			if (closestEnemy->health < 1) {
				auto const temp = *closestEnemy;
				*closestEnemy = enemyUnits.back();
				enemyUnits.pop_back();
				unitDeath(temp, enemyUnits);
			}

			didSomething_ = true;
		}
		else if (closestEnemy != enemyUnits.end() && sqrt(closestDist) > fu.speed) {
			auto const dx = closestEnemy->x - fu.x;
			auto const dy = closestEnemy->y - fu.y;

			fu.x += static_cast<int>(dx * (fu.speed / sqrt(dx * dx + dy * dy)));
			fu.y += static_cast<int>(dy * (fu.speed / sqrt(dx * dx + dy * dy)));

			didSomething_ = true;
		}
	}

	void FastAPproximation::medicsim(const FAPUnit &fu, std::vector<FAPUnit> &friendlyUnits) {
		auto closestHealable = friendlyUnits.end();
		auto closestDist = 0;

		for (auto it = friendlyUnits.begin(); it != friendlyUnits.end(); ++it) {
			if (it->isOrganic && it->health < it->maxHealth && !it->didHealThisFrame) {
				auto const d = distButNotReally(fu, *it);
				if (closestHealable == friendlyUnits.end() || d < closestDist) {
					closestHealable = it;
					closestDist = d;
				}
			}
		}

		if (closestHealable != friendlyUnits.end()) {
			fu.x = closestHealable->x;
			fu.y = closestHealable->y;

			closestHealable->health += 150;

			if (closestHealable->health > closestHealable->maxHealth)
				closestHealable->health = closestHealable->maxHealth;

			closestHealable->didHealThisFrame = true;
		}
	}

	bool FastAPproximation::suicideSim(const FAPUnit &fu, std::vector<FAPUnit> &enemyUnits) {
		auto closestEnemy = enemyUnits.end();
		auto closestDist = 0;

		for (auto enemyIt = enemyUnits.begin(); enemyIt != enemyUnits.end(); ++enemyIt) {
			if (enemyIt->flying) {
				if (fu.airDamage) {
					auto const d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.airMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
			else {
				if (fu.groundDamage) {
					const auto d = distButNotReally(fu, *enemyIt);
					if ((closestEnemy == enemyUnits.end() || d < closestDist) &&
						d >= fu.groundMinRange) {
						closestDist = d;
						closestEnemy = enemyIt;
					}
				}
			}
		}

		if (closestEnemy != enemyUnits.end() && sqrt(closestDist) <= fu.speed) {
			if (closestEnemy->flying)
				dealDamage(*closestEnemy, fu.airDamage, fu.airDamageType);
			else
				dealDamage(*closestEnemy, fu.groundDamage, fu.groundDamageType);

			if (closestEnemy->health < 1) {
				auto const temp = *closestEnemy;
				*closestEnemy = enemyUnits.back();
				enemyUnits.pop_back();
				unitDeath(temp, enemyUnits);
			}

			didSomething_ = true;
			return true;
		}
		else if (closestEnemy != enemyUnits.end() && sqrt(closestDist) > fu.speed) {
			auto const dx = closestEnemy->x - fu.x;
			auto const dy = closestEnemy->y - fu.y;

			fu.x += static_cast<int>(dx * (fu.speed / sqrt(dx * dx + dy * dy)));
			fu.y += static_cast<int>(dy * (fu.speed / sqrt(dx * dx + dy * dy)));

			didSomething_ = true;
		}

		return false;
	}

	void FastAPproximation::isimulate() {
		const auto simUnit = [this](auto &unit, auto &friendly, auto &enemy) {
			if (isSuicideUnit(unit->unitType)) {
				auto const unitDied = suicideSim(*unit, enemy);
				if (unitDied)
					unit = friendly.erase(unit);
				else ++unit;
			}
			else {
				if (unit->unitType == BWAPI::UnitTypes::Terran_Medic)
					medicsim(*unit, friendly);
				else
					unitsim(*unit, enemy);
				++unit;
			}
		};

		for (auto fu = player1.begin(); fu != player1.end();) {
			simUnit(fu, player1, player2);
		}

		for (auto fu = player2.begin(); fu != player2.end();) {
			simUnit(fu, player2, player1);
		}

		const auto updateUnit = [](FAPUnit &fu) {
			if (fu.attackCooldownRemaining)
				--fu.attackCooldownRemaining;
			if (fu.didHealThisFrame)
				fu.didHealThisFrame = false;

			if (fu.unitType.getRace() == BWAPI::Races::Zerg) {
				if (fu.health < fu.maxHealth)
					fu.health += 4;
				if (fu.health > fu.maxHealth)
					fu.health = fu.maxHealth;
			}
			else if (fu.unitType.getRace() == BWAPI::Races::Protoss) {
				if (fu.shields < fu.maxShields)
					fu.shields += 7;
				if (fu.shields > fu.maxShields)
					fu.shields = fu.maxShields;
			}
		};

		for (auto &fu : player1)
			updateUnit(fu);

		for (auto &fu : player2)
			updateUnit(fu);
	}

	void FastAPproximation::unitDeath(const FAPUnit &fu, std::vector<FAPUnit> &itsFriendlies) {
		if (fu.unitType == BWAPI::UnitTypes::Terran_Bunker) {
			convertToUnitType(fu, BWAPI::UnitTypes::Terran_Marine);

			for (unsigned i = 0; i < 4; ++i)
				itsFriendlies.push_back(fu);
		}
	}

	void FastAPproximation::convertToUnitType(const FAPUnit &fu, BWAPI::UnitType const ut) {
		UnitInfo ed;
		ed.lastPosition = { fu.x, fu.y };
		ed.player = fu.player;
		ed.type = ut;

		FAPUnit funew(ed);
		funew.attackCooldownRemaining = fu.attackCooldownRemaining;
		funew.elevation = fu.elevation;

		fu = funew;
	}

	FastAPproximation::FAPUnit::FAPUnit(BWAPI::Unit const u) : FAPUnit(UnitInfo(u)) {}

	FastAPproximation::FAPUnit::FAPUnit(UnitInfo ed)
		: x(ed.lastPosition.x), y(ed.lastPosition.y),

		health(ed.lastHealth),

		maxHealth(ed.type.maxHitPoints()), armor(ed.player->armor(ed.type)),

		shields(ed.lastShields),
		shieldArmor(ed.player->getUpgradeLevel(BWAPI::UpgradeTypes::Protoss_Plasma_Shields)),
		maxShields(ed.type.maxShields()),
		speed(ed.player->topSpeed(ed.type)), flying(ed.type.isFlyer()),

		unitSize(ed.type.size()),
		groundDamage(ed.player->damage(ed.type.groundWeapon())),
		groundCooldown(ed.type.groundWeapon().damageFactor() &&
			ed.type.maxGroundHits()
			? ed.player->weaponDamageCooldown(ed.type) /
			(ed.type.groundWeapon().damageFactor() *
				ed.type.maxGroundHits())
			: 0),
		groundMaxRange(ed.player->weaponMaxRange(ed.type.groundWeapon())),
		groundMinRange(ed.type.groundWeapon().minRange()),

		groundDamageType(ed.type.groundWeapon().damageType()),
		airDamage(ed.player->damage(ed.type.airWeapon())),
		airCooldown(ed.type.airWeapon().damageFactor() &&
			ed.type.maxAirHits()
			? ed.type.airWeapon().damageCooldown() /
			(ed.type.airWeapon().damageFactor() *
				ed.type.maxAirHits())
			: 0),
		airMaxRange(ed.player->weaponMaxRange(ed.type.airWeapon())),
		airMinRange(ed.type.airWeapon().minRange()),

		airDamageType(ed.type.airWeapon().damageType()), unitType(ed.type),
		player(ed.player),
		isOrganic(ed.type.isOrganic()), score(ed.type.destroyScore()) {
		static auto nextId = 0;
		id = nextId++;

		switch (ed.type) {
		case BWAPI::UnitTypes::Protoss_Carrier:
			groundDamage = ed.player->damage(
				BWAPI::UnitTypes::Protoss_Interceptor.groundWeapon());

			if (ed.unit && ed.unit->isVisible()) {
				auto const interceptorCount = ed.unit->getInterceptorCount();
				if (interceptorCount) {
					groundCooldown = static_cast<int>(round(37.0f / interceptorCount));
				}
				else {
					groundDamage = 0;
					groundCooldown = 5;
				}
			}
			else {
				if (ed.player) {
					groundCooldown =
						static_cast<int>(round(37.0f / (ed.player->getUpgradeLevel(BWAPI::UpgradeTypes::Carrier_Capacity) ? 8 : 4)));
				}
				else {
					groundCooldown = static_cast<int>(round(37.0f / 8));
				}
			}

			groundDamageType = BWAPI::UnitTypes::Protoss_Interceptor.groundWeapon().damageType();
			groundMaxRange = 32 * 8;

			airDamage = groundDamage;
			airDamageType = groundDamageType;
			airCooldown = groundCooldown;
			airMaxRange = groundMaxRange;
			break;

		case BWAPI::UnitTypes::Terran_Bunker:
			groundDamage = ed.player->damage(BWAPI::WeaponTypes::Gauss_Rifle);
			groundCooldown = BWAPI::UnitTypes::Terran_Marine.groundWeapon().damageCooldown() / 4;
			groundMaxRange = ed.player->weaponMaxRange(BWAPI::UnitTypes::Terran_Marine.groundWeapon()) + 32;

			airDamage = groundDamage;
			airCooldown = groundCooldown;
			airMaxRange = groundMaxRange;
			break;

		case BWAPI::UnitTypes::Protoss_Reaver:
			groundDamage = ed.player->damage(BWAPI::WeaponTypes::Scarab);
			break;
		}

		if (ed.unit && ed.unit->isStimmed()) {
			groundCooldown /= 2;
			airCooldown /= 2;
		}

		if (ed.unit && ed.unit->isVisible() && !ed.unit->isFlying())
			elevation = BWAPI::Broodwar->getGroundHeight(ed.unit->getTilePosition());

		groundMaxRange *= groundMaxRange;
		groundMinRange *= groundMinRange;
		airMaxRange *= airMaxRange;
		airMinRange *= airMinRange;

		health <<= 8;
		maxHealth <<= 8;
		shields <<= 8;
		maxShields <<= 8;
	}

	const FastAPproximation::FAPUnit &FastAPproximation::FAPUnit::  // NOLINT
		operator=(const FAPUnit &other) const {
		x = other.x, y = other.y;
		health = other.health, maxHealth = other.maxHealth;
		shields = other.shields, maxShields = other.maxShields;
		speed = other.speed, armor = other.armor, flying = other.flying,
			unitSize = other.unitSize;
		groundDamage = other.groundDamage, groundCooldown = other.groundCooldown,
			groundMaxRange = other.groundMaxRange, groundMinRange = other.groundMinRange,
			groundDamageType = other.groundDamageType;
		airDamage = other.airDamage, airCooldown = other.airCooldown,
			airMaxRange = other.airMaxRange, airMinRange = other.airMinRange,
			airDamageType = other.airDamageType;
		score = other.score;
		attackCooldownRemaining = other.attackCooldownRemaining;
		unitType = other.unitType;
		isOrganic = other.isOrganic;
		didHealThisFrame = other.didHealThisFrame;
		elevation = other.elevation;
		player = other.player;

		return *this;
	}

	bool FastAPproximation::FAPUnit::operator<(const FAPUnit &other) const {
		return id < other.id;
	}
} // namespace Neolib