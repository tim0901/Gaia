#ifndef RANDOM_H
#define RANDOM_H

import sml;
using namespace sml;
#include "Pi.h"
#include <random>

class Rng {
public:
	Rng() {
		std::random_device r;
		rng = std::mt19937_64(r());
		doubleDis01 = std::uniform_real_distribution<double>(0.0, 1.0);
		angleDis2Pi = std::uniform_real_distribution<double>(0.0, M_2PI);
	}
	double RandomDouble() {
		return doubleDis01(rng);
	}

	// Return a double between 0.0 and 2 * M_PI
	double RandomAngle2Pi() {
		return angleDis2Pi(rng);
	}

	std::unique_ptr<double []> RandomDouble(const int n) {
		std::unique_ptr<double[]> ret = std::make_unique<double[]>(n);
		for (int i = 0; i < n; i++)
			ret[i] = RandomDouble();
		return std::move(ret);
	}

	std::unique_ptr<double []> RandomAngle2Pi(const int n) {
		std::unique_ptr<double[]> ret = std::make_unique<double[]>(n);
		for (int i = 0; i < n; i++)
			ret[i] = RandomAngle2Pi();
		return std::move(ret);
	}

	// Sampling distributions
	Vec2d RandomInUnitDisk();
	Vec3d RandomInUnitDiskXY();
	Vec3d RandomInUnitDiskXZ();
	Vec3d RandomInUnitDiskYZ();
	Vec3d RandomInHemisphere();
	Vec3d RandomCosineHemisphere();

	// PDFs for sampling distributions
	double RandomInUnitDiskPDF();
	double RandomInUnitDiskXYPDF();
	double RandomInUnitDiskXZPDF();
	double RandomInUnitDiskYZPDF();
	double RandomInHemispherePDF() { return 1 / M_2PI; };
	double RandomCosineHemispherePDF(const Vec3d& sampledPosition, const Vec3d& normal);

	std::mt19937_64 rng;
	std::uniform_real_distribution<double> doubleDis01;
	std::uniform_real_distribution<double> angleDis2Pi;
};

extern thread_local Rng rng;


#endif // !RANDOM_H