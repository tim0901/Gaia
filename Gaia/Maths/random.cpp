
#include "Random.h"

thread_local Rng rng;

// Generate points on a unit radius disk. 
// Uses concentric mapping to preserve stratification
// Points are between x,y = -1 and x,y = 1
Vec2d Rng::RandomInUnitDisk() {
	double phi, r;

	double a = 2.0 * RandomDouble() - 1.0;
	double b = 2.0 * RandomDouble() - 1.0;

	if (a * a > b * b) {
		r = a;
		phi = M_PI_4 * (b / a);
	}
	else {
		r = b;
		phi = M_PI_2 - (M_PI_4 * (a / b));
	}
	return Vec2d(r * cos(phi), r * sin(phi));
}

Vec3d Rng::RandomInUnitDiskXY() {
	double phi, r;

	double a = 2.0 * RandomDouble() - 1.0;
	double b = 2.0 * RandomDouble() - 1.0;

	if (a * a > b * b) {
		r = a;
		phi = M_PI_4 * (b / a);
	}
	else {
		r = b;
		phi = M_PI_2 - (M_PI_4 * (a / b));
	}
	return Vec3d(r * cos(phi), r * sin(phi), 0.0);
}

Vec3d Rng::RandomInUnitDiskXZ() {
	double phi, r;

	double a = 2.0 * RandomDouble() - 1.0;
	double b = 2.0 * RandomDouble() - 1.0;

	if (a * a > b * b) {
		r = a;
		phi = M_PI_4 * (b / a);
	}
	else {
		r = b;
		phi = M_PI_2 - (M_PI_4 * (a / b));
	}
	return Vec3d(r * cos(phi), 0.0, r * sin(phi));
}

Vec3d Rng::RandomInUnitDiskYZ() {
	double phi, r;

	double a = 2.0 * RandomDouble() - 1.0;
	double b = 2.0 * RandomDouble() - 1.0;

	if (a * a > b * b) {
		r = a;
		phi = M_PI_4 * (b / a);
	}
	else {
		r = b;
		phi = M_PI_2 - (M_PI_4 * (a / b));
	}
	return Vec3d(0.0, r * cos(phi), r * sin(phi));
}

Vec3d Rng::RandomInHemisphere() {
	double z = RandomDouble();
	double r = sqrt(std::max(0.0, 1.0 - z * z));
	double phi = M_2PI * RandomDouble(); 
	return Vec3d(r * cos(phi), r * sin(phi), z);
}

Vec3d Rng::RandomCosineHemisphere() {
	Vec2d p = RandomInUnitDisk();
	double z = sqrt(std::max(0.0, 1 - p.x() * p.x() - p.y() * p.y()));
	return Vec3d(p.x(), p.y(), z);
}

double Rng::RandomCosineHemispherePDF(const Vec3d& sampledPosition, const Vec3d& normal) {
	return abs(dot(normal, sampledPosition)) * M_1_PI;
}