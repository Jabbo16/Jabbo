#pragma once
#include <BWAPI.h>
#include <set>
namespace Jabbo {
	using namespace BWAPI;
	using namespace std;

	class Cluster
	{
	public:
		set<Unit> units{};
		double modeX{};
		double modeY{};

		void updateCentroid();
		pair<double, double> mode() const;

		bool operator==(const Cluster c) const
		{
			return modeX == c.modeX && modeY == c.modeY;
		}
	};

	class Clustering
	{
		struct UnitPos
		{
			UnitPos(const Unit unit, const double x, const double y)
			{
				this->unit = unit, this->x = x, this->y = y;
			}

			UnitPos& operator=(const vector<UnitPos>::pointer unitPos)
			{
				this->unit = unitPos->unit;
				this->x = unitPos->x;
				this->y = unitPos->y;
				return *this;
			}
			Unit unit{};
			double x{};
			double y{};
		};

	private:
		const double radius_ = pow(UnitTypes::Terran_Siege_Tank_Siege_Mode.groundWeapon().maxRange(), 2);
		vector<UnitPos> points_{};
		double gaussianKernel(double distanceSquared, double bandwidth) const;
		double euclideanDistanceSquared(double point1X, double point1Y, double point2X, double point2Y) const;
		vector<pair<double, double>> getNeighbours(Unit unit, double pointX, double pointY);
	public:
		long time{};
		explicit Clustering(vector<Unit> units);
		vector<Cluster> run();
	};
}