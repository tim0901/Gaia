#include "Glass.h"

Vec3d Glass::SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const {


	Vec3d inDirection = unit_vector(r.direction);
	Vec3d unitNormal = unit_vector(iRec.normal);

	double eta_t = 1.0;
	double eta_i = 1.0;

	// Is the incident ray coming from inside the material?
	if (dot(inDirection, unitNormal) > 0.0) {
		// Outside
		eta_t = refractiveIndex;
	}
	else {
		// Inside
		eta_i = refractiveIndex;
		unitNormal = -unitNormal;
	}
	double eta = eta_i / eta_t;

	// Use Fresnel to determine whether the ray reflects or refracts off of the surface
	double reflectChance = Fresnel(inDirection, unitNormal, eta_t, eta_i);

	if (rng.RandomDouble() > reflectChance) {
		// Refract
		Vec3d outDirection;
		if (Refract(inDirection, unitNormal, eta, outDirection)) {
			sRec.scatteredRay = Ray(iRec.p, outDirection);
			sRec.scatteringPDF = 1.0 - reflectChance;
			sRec.specular = true;
			return R * Vec3d(1.0 - reflectChance) / std::abs(dot(sRec.scatteredRay.direction, unitNormal));
		}
	}

	// Reflect

	Vec3d outBound = Reflect(inDirection, unitNormal);

	sRec.scatteredRay = Ray(iRec.p, outBound);
	sRec.scatteringPDF = reflectChance;
	sRec.specular = true;

	return Vec3d( R * reflectChance / std::abs(dot(outBound, unitNormal)));
}


void Glass::SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const {


	Vec3d inDirection = unit_vector(r.direction);
	Vec3d unitNormal = unit_vector(iRec.normal);

	double eta_t = 1.0;
	double eta_i = 1.0;

	// Is the incident ray coming from inside the material?
	if (dot(inDirection, unitNormal) > 0.0) {
		// Outside
		eta_t = refractiveIndex;
	}
	else {
		// Inside
		eta_i = refractiveIndex;
		unitNormal = -unitNormal;
	}
	double eta = eta_i / eta_t;

	// Use Fresnel to determine whether the ray reflects or refracts off of the surface
	double reflectChance = Fresnel(inDirection, unitNormal, eta_t, eta_i);

	if (rng.RandomDouble() > reflectChance) {
		// Refract
		Vec3d outDirection;
		if (Refract(inDirection, unitNormal, eta, outDirection)) {
			sRec.scatteredRay = Ray(iRec.p, outDirection);
			sRec.scatteringPDF = 1.0 - reflectChance;
			sRec.specular = true;
		}
	}

	// Reflect
	Vec3d outBound = Reflect(inDirection, unitNormal);

	sRec.scatteredRay = Ray(iRec.p, unit_vector(outBound));
	sRec.scatteringPDF = reflectChance;
	sRec.specular = true;
}

Vec3d Glass::ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

	return Vec3d(0.0, 0.0, 0.0);
}