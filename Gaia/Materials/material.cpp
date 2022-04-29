#include "Material.h"

Vec3d Reflect(const Vec3d& inDirection, const Vec3d& normal) {
	return unit_vector(2.0 * dot(inDirection, normal) * normal - inDirection);
}

bool Refract(const Vec3d& inDirection, const Vec3d& normal, const double& eta, Vec3d& outDirection) {

	Vec3d inBound = -inDirection;
	double cosTheta_i = dot(inDirection, normal);
	double descriminant = 1.0 - eta * eta * (1.0 - cosTheta_i * cosTheta_i);
	if (descriminant > 0.0) {
		outDirection = unit_vector(eta * inBound + (eta * cosTheta_i - sqrt(descriminant)) * normal);
		return true;
	}
	return false;
}

double Fresnel(Vec3d w_i, Vec3d n, double eta_t, double eta_i) {
	// Fresnel
	double eta_t2 = eta_t * eta_t;
	double eta_i2 = eta_i * eta_i;

	double c = abs(dot(w_i, n));

	double gSquared = (eta_t2 / eta_i2) - 1.0 + (c * c);
	if (gSquared < 0.0) { // Return 1 if g is imaginary - total internal reflection
		return 1.0;
	}

	double g = sqrt(gSquared);
	return 0.5 * (((g - c) * (g - c)) / ((g + c) * (g + c))) * (1.0 + (((c * (g + c) - 1.0) * (c * (g + c) - 1.0)) / ((c * (g - c) + 1.0) * (c * (g - c) + 1.0))));

}