#include "Clustering.hpp"
#include <chrono>

using namespace BWAPI;
namespace Jabbo {
	void Cluster::updateCentroid()
	{
		if (this->units.empty()) return;
		const double size = this->units.size();
		auto x = 0.0;
		auto y = 0.0;
		for (auto u : this->units)
		{
			x += u->getPosition().x;
			y += u->getPosition().y;
		}
		this->modeX = x / size;
		this->modeY = y / size;
	}

	pair<double, double> Cluster::mode() const
	{
		return pair<double, double>{ modeX, modeY };
	}

	Clustering::Clustering(vector<Unit> units)
	{
		for (auto u : units)
		{
			if (!u->isVisible() || !u->isCompleted()) continue;
			const auto p = u->getPosition();
			this->points_.emplace_back(UnitPos(u, p.x, p.y));
		}
	}

	double Clustering::gaussianKernel(const double distanceSquared, const double bandwidth) const
	{
		return exp(-1.0 / 2.0 * distanceSquared / (bandwidth * bandwidth));
	}
	double Clustering::euclideanDistanceSquared(const double point1X, const double point1Y, const double point2X, const double point2Y) const
	{
		return pow(point1X - point2X, 2.0) + pow(point1Y - point2Y, 2.0);
	}

	vector<pair<double, double>> Clustering::getNeighbours(const Unit unit, const double pointX, const double pointY)
	{
		vector<pair<double, double>> neighbours{};
		for (auto u : points_) {
			if (unit == u.unit) continue;
			const auto dist = euclideanDistanceSquared(pointX, pointY, u.x, u.y);
			if (dist <= radius_) neighbours.emplace_back(pair<double, double>(u.x, u.y));
		}
		return neighbours;
	}

	vector<Cluster> Clustering::run()
	{
		const auto start = std::chrono::high_resolution_clock::now();
		vector<Cluster> clusters{};
		if (points_.empty()) return clusters;
		const auto iterations = 15;
		const auto bandwidth = 2;
		for (auto iter = 0; iter < iterations; iter++) {
# pragma omp parallel for
			for (auto i = 0; i < static_cast<signed int>(points_.size()); i++)
			{
				const auto aux = points_[i];
				const auto initialX = aux.x;
				const auto initialY = aux.y;
				auto neighbours = getNeighbours(aux.unit, initialX, initialY);
				double numeratorX = 0;
				double numeratorY = 0;
				double denominator = 0;
				for (const auto neighbour : neighbours) {
					const auto distanceSquared = euclideanDistanceSquared(neighbour.first, neighbour.second, initialX, initialY);
					const auto weight = gaussianKernel(distanceSquared, bandwidth);
					numeratorX += weight * neighbour.first;
					numeratorY += weight * neighbour.second;
					denominator += weight;
				}
				auto newPointX = numeratorX / denominator;
				auto newPointY = numeratorY / denominator;
				if (neighbours.empty()) {
					newPointX = initialX;
					newPointY = initialY;
				}
				if (isinf(newPointX) || isnan(newPointX)) newPointX = initialX;  // HACK
				if (isinf(newPointY) || isnan(newPointY)) newPointY = initialY; // HACK
				points_[i] = UnitPos(aux.unit, newPointX, newPointY);
			}
		}
		for (auto i : points_) {
			unsigned int c = 0;
			for (const auto& cluster : clusters) {
				if (euclideanDistanceSquared(i.x, i.y, cluster.modeX, cluster.modeY) <= 400.0 * 400.0) break;
				c++;
			}
			if (c == clusters.size()) {
				Cluster cluster{};
				cluster.modeX = i.x;
				cluster.modeY = i.y;
				clusters.emplace_back(cluster);
			}
			clusters[c].units.emplace(i.unit);
			clusters[c].updateCentroid();
		}
		const auto end = std::chrono::high_resolution_clock::now();
		time = long(std::chrono::duration_cast<chrono::milliseconds>(end - start).count());
		return clusters;
	}
}