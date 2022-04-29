#include "TrowbridgeReitz.h"

Vec3d TrowbridgeReitz::SampleAndEvaluate(const Ray& r, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

	Vec3d unitInDirection = unit_vector(r.direction);
	Vec3d unitSurfaceNormal = unit_vector(iRec.normal);
	double eta_t = 1.0;
	double eta_i = 1.0;

	int sign = 0;

	// Is the incident ray coming from inside the material?
	if (dot(unitInDirection, unitSurfaceNormal) > 0.0) {
		// Outside
		eta_t = refractiveIndex;
		sign = 1;
	}
	else {
		// Inside
		eta_i = refractiveIndex;
		unitSurfaceNormal = -unitSurfaceNormal;
		sign = -1;
	}
	double eta = eta_i / eta_t;


	// First we use two random numbers to generate a microfacced normal sample (m)

	double r1 = rng.RandomDouble();

	double tanTheta2 = ((alphaG * alphaG * r1) / (1.0 - r1));
	double cosTheta = 1.0 / sqrt(1 + tanTheta2);
	double sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
	double phi_m = M_2PI * rng.RandomDouble();;

	Vec3d microfacetNormal = unit_vector(Vec3d(sinTheta * cos(phi_m), sinTheta * sin(phi_m), cosTheta)); // m

	// We then use Fresnel to determine whether the ray reflects or refracts off of the sampled microfacet
	double reflectChance = Fresnel(unitInDirection, microfacetNormal, eta_t, eta_i);

	// We then use Fresnel to determine whether the ray reflects or refracts off of the sampled microfacet
	Vec3d sampledDirection(0.0, 0.0, 0.0);
	double c = dot(unitInDirection, microfacetNormal);
	double f = 0.0;
	if (reflectChance > rng.RandomDouble()) {
		// Refract
		std::cout << "SPAWN REFRACT\n";
		if (Refract(unitInDirection, microfacetNormal, eta, sampledDirection)) {

			Vec3d h_t = unit_vector(-(eta_i * unitInDirection + eta_t * sampledDirection));

			double iDoth = dot(unitInDirection, h_t);
			double oDoth = dot(sampledDirection, h_t);

			f = ((abs(iDoth) * abs(oDoth)) / (abs(dot(unitInDirection, unitSurfaceNormal)) * abs(dot(sampledDirection, unitSurfaceNormal))))
				* (eta_t * eta_t * (1.0 - Fresnel(unitInDirection, h_t, eta_t, eta_i)) * G(unitInDirection, sampledDirection, h_t, unitSurfaceNormal) * D(h_t, unitSurfaceNormal))
				/ pow((eta_i * iDoth + eta_t * oDoth), 2.0);

			// Construct scattered ray and sample the PDF
			sRec.scatteredRay = Ray(iRec.p, sampledDirection);
			sRec.scatteringPDF = EvaluatePDFFromDirections(unitInDirection, sRec.scatteredRay.direction, unitSurfaceNormal);
			sRec.specular = true;
			Vec3d R = reflectance->SampleTexture(iRec);
			std::cout << R * f << "\n";
			return R * f;

		}
	}

	// Reflect
	std::cout << "REFLECT\n";
	sampledDirection = Reflect(unitInDirection, microfacetNormal);
	//std::cout << "In: " << inDirection << " Out: " << sampledDirection << " Microfacet normal: " << microfacetNormal << " iRec.p: " << iRec.p << "\n";
	sRec.scatteredRay = Ray(iRec.p, sampledDirection);

	Vec3d h_r = unit_vector(sign * (unitInDirection + sampledDirection));
	f = (Fresnel(unitInDirection, h_r, eta_t, eta_i) * G(unitInDirection, sampledDirection, h_r, unitSurfaceNormal) * D(h_r, unitSurfaceNormal)) / (4.0 * abs(dot(unitInDirection, unitSurfaceNormal)) * abs(dot(sampledDirection, unitSurfaceNormal)));
	
	// Construct scattered ray and sample the PDF
	sRec.scatteringPDF = EvaluatePDFFromDirections(unitInDirection, sRec.scatteredRay.direction, unitSurfaceNormal);
	sRec.specular = true;
	Vec3d R = reflectance->SampleTexture(iRec);
	std::cout << R * f << "\n";
	return R * f;



	/*
	if (dot(-unitInDirection, unitOutDirection) > 0.0) {
		// Refracted
		Vec3d h_t = unit_vector(-(eta_i * unitInDirection + eta_t * unitOutDirection));
		//std::cout << "REFRACT" << sRec.rayNumber << "\n";
		double iDoth = dot(unitInDirection, h_t);
		double oDoth = dot(unitOutDirection, h_t);

		f = ((abs(iDoth) * abs(oDoth)) / (abs(dot(unitInDirection, iRec.normal)) * abs(dot(unitOutDirection, iRec.normal))))
			* (eta_t * eta_t * (1.0 - Fresnel(unitInDirection, h_t, eta_t, eta_i)) * G(unitInDirection, unitOutDirection, h_t, iRec.normal) * D(h_t, iRec.normal))
			/ pow((eta_i * iDoth + eta_t * oDoth), 2.0);
	}
	else {
		// Reflected
		Vec3d h_r = unit_vector(sign * (unitInDirection + unitOutDirection));
		//std::cout << "REFLECT" << sRec.rayNumber << "\n";

		//std::cout << F(unitInDirection, h_r, eta_t, eta_i) << " " << G(unitInDirection, outDirection, h_r, iRec.normal) << " " << D(h_r, iRec.normal) << std::endl;

		f = (Fresnel(unitInDirection, h_r, eta_t, eta_i) * G(unitInDirection, unitOutDirection, h_r, iRec.normal) * D(h_r, iRec.normal)) / (4.0 * abs(dot(unitInDirection, iRec.normal)) * abs(dot(unitOutDirection, iRec.normal)));

	}*/
	//	std::cout << f << std::endl;
}

void TrowbridgeReitz::SampleDirection(const Ray& r, const IntersectionRecord& iRec, ScatteringRecord& sRec) const {

	Vec3d inDirection = unit_vector(r.direction);
	Vec3d unitNormal = unit_vector(iRec.normal);

	double eta_t = 1.0;
	double eta_i = 1.0;

	int sign = 0;

	// Is the incident ray coming from inside the material?
	if (dot(inDirection, unitNormal) > 0.0) {
		// Outside
		eta_t = refractiveIndex;
		sign = 1;
	}
	else {
		// Inside
		eta_i = refractiveIndex;
		unitNormal = -unitNormal;
		sign = -1;
	}
	double eta = eta_i / eta_t;

	// First we use two random numbers to generate a microfacced normal sample (m)

	double r1 = rng.RandomDouble();

	double tanTheta2 = ((alphaG * alphaG * r1) / (1.0 - r1));
	double cosTheta = 1.0 / sqrt(1 + tanTheta2);
	double sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));
	double phi_m = M_2PI * rng.RandomDouble();;

	Vec3d microfacetNormal = Vec3d(sinTheta * cos(phi_m), sinTheta * sin(phi_m), cosTheta); // m
	microfacetNormal = unit_vector(microfacetNormal);


	// We then use Fresnel to determine whether the ray reflects or refracts off of the sampled microfacet
	Vec3d sampledDirection(0.0, 0.0, 0.0);
	double c = dot(inDirection, microfacetNormal);

	double reflectChance = Fresnel(inDirection, microfacetNormal, eta_t, eta_i);

	if (reflectChance > rng.RandomDouble()) {
		// Reflect
		//std::cout << "SPAWN REFLECT" << sRec.rayNumber << "\n";
		//std::cout << "Reflect chance: " << reflectChance << "\n";
		sampledDirection = 2.0 * abs(c) * microfacetNormal - inDirection;
		//std::cout << "In: " << inDirection << " Out: " << sampledDirection << " Microfacet normal: " << microfacetNormal << " iRec.p: " << iRec.p << "\n";
		sRec.scatteredRay = Ray(iRec.p, unit_vector(sampledDirection));
	}
	else {
		// Refract
		//std::cout << "SPAWN REFRACT" << sRec.rayNumber << "\n";
		//std::cout << "Reflect chance: " << reflectChance << "\n";
		sampledDirection = ((eta * c) - sign * sqrt(1.0 + (eta * (c * c - 1.0)))) * microfacetNormal - (eta * inDirection);
		//std::cout << "In: " << inDirection << " Out: " << sampledDirection << " Microfacet normal: " << microfacetNormal << " iRec.p: " << iRec.p << "\n";

		// Construct new ray
		sRec.scatteredRay = Ray(iRec.p, unit_vector(sampledDirection));
	}

	// Construct scattered ray and sample the PDF
	sRec.scatteringPDF = EvaluatePDFFromDirections(inDirection, sRec.scatteredRay.direction, iRec.normal);
	sRec.microfacetNormal = microfacetNormal;
	sRec.specular = true;
}

Vec3d TrowbridgeReitz::ScatteredRadiance(const Vec3d& inDirection, const Vec3d& outDirection, IntersectionRecord& iRec, ScatteringRecord& sRec) const {

	return Vec3d(0.0, 0.0, 0.0);

	Vec3d unitInDirection = unit_vector(inDirection);
	Vec3d unitOutDirection = unit_vector(outDirection);

	double eta_t = 1.0;
	double eta_i = 1.0;

	// Is the incident ray coming from inside the material?
	double sign = Signum(dot(unitInDirection, iRec.normal));
	if (sign > 0.0) {
		// Outside
		eta_t = refractiveIndex;
	}
	else {
		// Inside
		eta_i = refractiveIndex;
	}
	double eta = eta_i / eta_t;

	double f = 0.0;
	if (dot(-unitInDirection, unitOutDirection) > 0.0) {
		// Refracted
		Vec3d h_t = unit_vector(-(eta_i * unitInDirection + eta_t * unitOutDirection));
		//std::cout << "REFRACT" << sRec.rayNumber << "\n";
		double iDoth = dot(unitInDirection, h_t);
		double oDoth = dot(unitInDirection, h_t);

		f = ((abs(iDoth) * abs(oDoth)) / (abs(dot(unitInDirection, iRec.normal)) * abs(dot(unitOutDirection, iRec.normal))))
			* (eta_t * eta_t * (1.0 - Fresnel(unitInDirection, h_t, eta_t, eta_i)) * G(unitInDirection, unitOutDirection, h_t, iRec.normal) * D(h_t, iRec.normal))
			/ pow((eta_i * iDoth + eta_t * oDoth), 2.0);
	}
	else {
		// Reflected
		Vec3d h_r = unit_vector(sign * (unitInDirection + unitOutDirection));
		//std::cout << "REFLECT" << sRec.rayNumber << "\n";

		//std::cout << F(unitInDirection, h_r, eta_t, eta_i) << " " << G(unitInDirection, outDirection, h_r, iRec.normal) << " " << D(h_r, iRec.normal) << std::endl;

		f = (Fresnel(unitInDirection, h_r, eta_t, eta_i) * G(unitInDirection, unitOutDirection, h_r, iRec.normal) * D(h_r, iRec.normal)) / (4.0 * abs(dot(unitInDirection, iRec.normal)) * abs(dot(unitOutDirection, iRec.normal)));

	}
	//	std::cout << f << std::endl;

	Vec3d R = reflectance->SampleTexture(iRec);
	return R * f;
}